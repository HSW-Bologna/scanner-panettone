#ifndef GETTONIERA_H_INCLUDED
#define GETTONIERA_H_INCLUDED

void gettoniera_init(void);
void gettoniera_reset_count(void);
int gettoniera_take_insert(void);
unsigned int gettoniera_get_count(void);
int gettoniera_get_pulse_level(void);
unsigned int gettoniera_get_count_ingresso(void);

#endif
