#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

unsigned long get_millis(void);
void          timer_init(void);
int           timer_second_passed(void);

#endif  /* TIMER_H_INCLUDED */
