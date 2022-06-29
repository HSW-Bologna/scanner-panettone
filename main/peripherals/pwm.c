#include <xc.h>
#include "model/model.h"

#include "hardwareprofile.h"

#include "pwm.h"

#define PERIOD(f) (FCY/f)


static __inline__ void enable_pwm1(void) {
    OC1CON1bits.OCM = 6; /* This selects and starts the Edge Aligned PWM mode*/
}


static __inline__ void disable_pwm1(void) {
    OC1CON1bits.OCM = 0; /* This selects and starts the Edge Aligned PWM mode*/
}


static __inline__ void enable_pwm2(void) {
    OC2CON1bits.OCM = 6; /* This selects and starts the Edge Aligned PWM mode*/
}


static __inline__ void disable_pwm2(void) {
    OC2CON1bits.OCM = 0; /* This selects and starts the Edge Aligned PWM mode*/
}



void pwm_init(void) {
    TRISBbits.TRISB4 = TRIS_OUTPUT; 
    
    OC1CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC1CON2 = 0;
    
    OC1CON1bits.OCTSEL = 0x07; /* This selects the peripheral clock as the clock input to the OC module */  
    OC1CON2bits.SYNCSEL = 0x1F; /* This selects the synchronization source as itself */

    
    OC2CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC2CON2 = 0;
    
    OC2CON1bits.OCTSEL = 0x07; /* This selects the peripheral clock as the clock input to the OC module */
    
    OC2CON2bits.SYNCSEL = 0x1F; /* This selects the synchronization source as itself */

}


void pwm_off(pwm_channel_t channel) {
    switch (channel) {
        case PWM_CHANNEL_OUT_STEP_MOTORE_P:
            disable_pwm1();
            break;
            
        case PWM_CHANNEL_OUT_STEP_MOTORE_S:
            disable_pwm2();
            break;
    }
}


void pwm_set(pwm_channel_t channel, uint16_t frequency) {
    switch (channel) {
        case PWM_CHANNEL_OUT_STEP_MOTORE_P:
            OC1R = PERIOD(frequency)/2; /* Half duty cycle */
            OC1RS = PERIOD(frequency); /* Determines the Period */
            enable_pwm1();
            break;
            
        case PWM_CHANNEL_OUT_STEP_MOTORE_S:
            OC2RS = PERIOD(frequency); /* Determines the Period */
            OC2R = PERIOD(frequency)/2; /* Half duty cycle */
            enable_pwm2();
            break;
    }
}