#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include <stdint.h>

#include "model/model.h"


typedef enum {
    CH_VEL_CESTO = 1,
    CH_VEL_VENTILAZIONE = 2,
} ch_vel_pwm_da_tt;

void pwm_init(void);

void pwm_set_test(uint8_t perc, uint8_t ch);
void pwm_set(model_t *p, uint8_t vel, uint8_t ch);

#endif
