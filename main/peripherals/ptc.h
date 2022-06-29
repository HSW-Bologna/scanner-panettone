#ifndef PTC_H_INCLUDED
#define PTC_H_INCLUDED

void ptc_init(void);
unsigned long ptc_read_input(int channel);
void ptc_read_temperature(void);
int ptc_get_temperature(void);
uint16_t ptc_get_adc_value(void);

#endif

