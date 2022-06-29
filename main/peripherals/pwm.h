#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include <stdint.h>

#include "model/model.h"


typedef enum {
    PWM_CHANNEL_OUT_STEP_MOTORE_P,
    PWM_CHANNEL_OUT_STEP_MOTORE_S,
} pwm_channel_t;


void pwm_init(void);
void pwm_set(pwm_channel_t channel, uint16_t frequency);
void pwm_off(pwm_channel_t channel);


#endif
