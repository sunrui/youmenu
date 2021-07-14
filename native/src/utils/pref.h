/*
 * instance ref manager
 *
 * @author rui.sun 2012_12_31 2:27
 */
#ifndef PREF_H
#define PREF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pref pref_t;

/*
 * pref_init - initialize object.
 * @pref_t: object in question.
 */
void pref_init(pref_t ** ref);

/**
 * pref_get - increment refcount for object.
 * @pref_t: object.
 */
int pref_get(pref_t * ref);

/*
 * pref_put - decrement refcount for object.
 * @pref_t: object.
 */
int pref_put(pref_t * ref);

/*
 * pref_count - check how many reference current.
 * @pref_t: object.
 */
int pref_count(pref_t * ref);

/*
 * pref_try_release - try to release if no reference current.
 *
 * return 1 if release ok or 0 instance still busy.
 */
int pref_try_release(pref_t ** ref);

/*
 * pref_release - handle and release object.
 */
void pref_release(pref_t ** ref);

/* may be you need pref_force_release or pref_force_release_timewait? 
    what an ugly code you do!!! */

#ifdef __cplusplus
}
#endif

#endif