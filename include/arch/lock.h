#ifndef lock_h
#define lock_h

#include <std/int.h>

typedef int lock_t;

bool try_lock(int *p);
void spin_lock(int *p);
void spin_unlock(int volatile *p);

#endif