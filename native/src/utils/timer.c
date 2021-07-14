/*
 * timer provider
 *
 * @author rui.sun Fri 2012-7-13 2:08
 */
#include "timer.h"
#include "list.h"

#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

struct st_timer
{
	pfn_timer_cb timer_cb;		/* handle timeout callback here */
	void * userdata;			/* timeout callback user data */
	int milliseconds;			/* set timeout in milliseconds */
	int remaining;				/* time remaining in last trigger time */
	int b_auto_delete;			/* whether auto delete this when time triggered */
	int b_sign_as_deleted;		/* sign this timer as deleted */
};

typedef struct timer_list
{
	st_timer_t timer;
	struct list_head list;
} timer_list_t;

struct timer
{
	struct list_head timer_list;		/* all sessions that registered to server */
	int stop_sign;						/* set to 1 if want to end loop */

	pthread_t thrd;						/* timer thread handle */
	pthread_mutex_t lock;               /* use for sleep exact time */
	struct timeval last_timeval;		/* save the last timeval between every tick */
	int tickfps;						/* tick frequency every time */
};

/*
 * timer tick in millisesecond
 *
 * @return
 *     the last timeval
 */
struct timeval timer_tick(timer_t * timer, int millisesecond)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	usleep(millisesecond);

	return now;
}

/*
 * count timer sub in milliseconds
 */
long my_timer_sub(struct timeval * now, struct timeval * last)
{
	long subtv;

	if ((now->tv_usec -= last->tv_usec) < 0)
	{    
		--now->tv_sec;
		now->tv_usec += 1000000;
	}

	now->tv_sec -= last->tv_sec;
	subtv = now->tv_sec * 1000 + now->tv_usec / 1000;

	return subtv;
}

/*
 * timer synchronize
 *
 * @return
 *     0 stopped ok
 *    -1 received a abort message
 */
int timer_synchronize(timer_t * timer, int milliseconds)
{
	struct timeval now;
	long elapsed_sec;

	timer->last_timeval = timer_tick(timer, milliseconds);
	gettimeofday(&now, NULL);

	/* check how long times elapsed during last tick */
	elapsed_sec = my_timer_sub(&now, &timer->last_timeval);

	/* synchronize all timer clock here */
	{
		struct list_head * pos, * q;
		timer_list_t * tmp;
        
        pthread_mutex_lock(&timer->lock);
		list_for_each_safe(pos, q, &timer->timer_list)
		{
			tmp = list_entry(pos, timer_list_t, list);
			if (tmp->timer.b_sign_as_deleted || timer->stop_sign)
			{
				list_del(pos);
				free(tmp);
				continue;
			}

			if (tmp->timer.remaining != 0)
			{
				tmp->timer.remaining -= elapsed_sec;
				if (tmp->timer.remaining < 0)
				{
					tmp->timer.remaining = 0;
				}
			}
		}
        pthread_mutex_unlock(&timer->lock);
	}

	if (timer->stop_sign)
	{
		return -1;
	}

	return 0;
}

/*
 * timer step tick in every milliseconds
 *
 * @return
 *     0 stopped ok
 *    -1 received a abort message
 */
int timer_step(timer_t * timer, int milliseconds)
{
	struct list_head * pos;
	timer_list_t * tmp;
	int ret;

	ret = timer_synchronize(timer, milliseconds);

	/* if we received a abort message tell timer_clock_proc at once */
	if (ret == -1) return ret;

	/* check which timer cb is timeout */
	list_for_each(pos, &timer->timer_list)
	{
		tmp = list_entry(pos, timer_list_t, list);
		if (tmp->timer.remaining == 0)
		{
			int b_abort_clock = 0;

			tmp->timer.timer_cb(tmp->timer.userdata, &b_abort_clock);
			if (b_abort_clock)
			{
				tmp->timer.b_sign_as_deleted = 1;
				continue;
			}

			/* re-check the time remaining again */
			tmp->timer.remaining = tmp->timer.milliseconds;
		}
	}

	return 0;
}

/*
 * start timer clock here, set timer_stop_sign 1 if you
 *  want to stop this timer process
 */
void * timer_clock_proc(void * param)
{
	timer_t * timer = (timer_t *)param;

	/* we just need only exact in timer_tickfps ms per step */
	while (timer_step(timer, timer->tickfps) != -1);

	return NULL;
}

/*
 * init timer environment, this function must be called
 *  before any timer_add can be used.
 *
 * @tickfps	tick frequency every time
 */
timer_t * timer_init(int tickfps)
{
	timer_t * timer;

	timer = (timer_t *)calloc(1, sizeof(timer_t));
	INIT_LIST_HEAD(&timer->timer_list);

	timer->stop_sign = 0;
	timer->tickfps = tickfps;
	gettimeofday(&timer->last_timeval, NULL);

	pthread_mutex_init(&timer->lock, NULL);
	pthread_create(&timer->thrd, NULL, timer_clock_proc, timer);

	return timer;
}

/*
 * add a new timer to timer clock
 *
 * @time_cb			trigger function when timeout reached.
 *
 * @userdata		time_cb user data
 *
 * @milliseconds	set trigger timeout milliseconds
 *
 * @b_auto_delete	if wanted automate delete timer when triggered
 *
 * @return
 *    timer_t instance which can be use for timer_del
 */
st_timer_t * timer_create(timer_t * timer, pfn_timer_cb time_cb, void * userdata, int milliseconds, int b_auto_delete)
{
	timer_list_t * item;

	item = (timer_list_t *)malloc(sizeof(timer_list_t));
	item->timer.b_auto_delete = b_auto_delete;
	item->timer.milliseconds = milliseconds;
	item->timer.remaining = milliseconds;
	item->timer.timer_cb = time_cb;
	item->timer.userdata = userdata;
	item->timer.b_sign_as_deleted = 0;

    pthread_mutex_lock(&timer->lock);
	list_add_tail(&item->list, &timer->timer_list);
    pthread_mutex_unlock(&timer->lock);

	return &item->timer;
}

/*
 * delete an exist timer
 *
 * @return
 *     0 delete ok
 *    -1 invalid timer
 */
int timer_del(timer_t * timer, st_timer_t * st_timer)
{
	struct list_head * pos;
	timer_list_t * tmp;

	if (timer == NULL || st_timer == NULL)
	{
		return -1;
	}
    
	list_for_each(pos, &timer->timer_list)
	{
		tmp = list_entry(pos, timer_list_t, list);
		if (&tmp->timer == st_timer)
		{
			tmp->timer.b_sign_as_deleted = 1;
			return 0;
		}
	}

	return -1;
}

/*
 * destroy timer when more used, it will also delete
 *  all registered timers.
 */
void timer_destroy(timer_t ** timer)
{
	if (timer != NULL && *timer != NULL)
	{
		/* stop timer thread first */
		(*timer)->stop_sign = 1;
		pthread_join((*timer)->thrd, NULL);
        
        {
            struct list_head * pos, * q;
            timer_list_t * tmp;
            
            pthread_mutex_lock(&(*timer)->lock);
            list_for_each_safe(pos, q, &(*timer)->timer_list)
            {
                tmp = list_entry(pos, timer_list_t, list);
                list_del(pos);
                free(tmp);
            }
            pthread_mutex_unlock(&(*timer)->lock);
        }
        
        pthread_mutex_destroy(&(*timer)->lock);
		assert(list_empty(&(*timer)->timer_list));
		free(*timer);
		*timer = NULL;
	}
}
