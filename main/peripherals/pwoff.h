#ifndef PWOFF_H_INCLUDED
#define	PWOFF_H_INCLUDED

void pwoff_init(void);
void pwoff_interrupt_enable(int i);
void pwoff_set_callback(void (*cb)(void));

#endif
