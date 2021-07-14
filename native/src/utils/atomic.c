/*
 * atomic
 *
 * Copyright (C) 2012-2013 qimery.com
 * @author rui.sun 2013-8-4
 */
#include "atomic.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct atomic atomic_t;

struct atomic
{
	int counter;
	pthread_mutex_t mutex;
};

/**
 * atomic init
 */
atomic_t * atomic_new()
{
	atomic_t * atomic = (atomic_t *)malloc(sizeof(atomic_t));
	atomic->counter = 0;
	pthread_mutex_init(&atomic->mutex, NULL);

	return atomic;
}

/**
 * atomic inc
 *
 * return
 *    atomic number BEFORE inc
 */
int atomic_inc(atomic_t * atomic)
{
	int counter;

	pthread_mutex_lock(&atomic->mutex);
	counter = atomic->counter++;
	pthread_mutex_unlock(&atomic->mutex);

	return counter;
}

/**
 * atomic dec
 *
 * return
 *    atomic number BEFORE dec
 */
int atomic_dec(atomic_t * atomic)
{
	int counter;

	pthread_mutex_lock(&atomic->mutex);
	counter = atomic->counter--;
	pthread_mutex_unlock(&atomic->mutex);

	return counter;
}

/**
 * atomic cur
 */
int atomic_cur(atomic_t * atomic)
{
	int counter;

	pthread_mutex_lock(&atomic->mutex);
	counter = atomic->counter;
	pthread_mutex_unlock(&atomic->mutex);

	return counter;
}

/**
 * atomic destroy
 */
void atomic_destroy(atomic_t ** atomic)
{
	if (atomic != NULL && *atomic != NULL)
	{
		pthread_mutex_destroy(&(*atomic)->mutex);
		free(*atomic);
	}
}