#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#define FOSC 32000000UL
#define FCY (FOSC/2)

void system_init(void);

#include <libpic30.h>
#include <xc.h>

#endif
