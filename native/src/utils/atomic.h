/*
 * atomic
 *
 * Copyright (C) 2012-2013 qimery.com
 * @author rui.sun 2013-8-4
 */
#ifndef ATOMIC_H
#define ATOMIC_H

typedef struct atomic atomic_t;

/**
 * atomic init
 */
atomic_t * atomic_new();

/**
 * atomic inc
 *
 * return
 *    atomic number BEFORE inc
 */
int atomic_inc(atomic_t * atomic);

/**
 * atomic dec
 *
 * return
 *    atomic number BEFORE dec
 */
int atomic_dec(atomic_t * atomic);

/**
 * atomic cur
 */
int atomic_cur(atomic_t * atomic);

/**
 * atomic destroy
 */
void atomic_destroy(atomic_t ** atomic);

#endif