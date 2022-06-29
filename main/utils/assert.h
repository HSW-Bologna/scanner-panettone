#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#ifdef PC_SIMULATOR
#include <assert.h>
#else
void assert(int expr);
#endif

#endif