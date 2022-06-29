/*
 * File:   ptc.c
 * Author: Virginia
 *
 * Created on 27 aprile 2021, 10.31
 */


#include "xc.h"
#include "hardwareprofile.h"
#include "ptc.h"

#define MINIMUM_AD_VALUE 1061L
#define MAXIMUM_AD_VALUE 2049L
#define MINIMUM_TEMP_VALUE -10L
#define MAXIMUM_TEMP_VALUE 140L

#define COEFF_M_TIMES(x) ((x * (MAXIMUM_TEMP_VALUE-MINIMUM_TEMP_VALUE))/(MAXIMUM_AD_VALUE-MINIMUM_AD_VALUE))
#define COEFF_Q (-MINIMUM_AD_VALUE*(MAXIMUM_TEMP_VALUE-MINIMUM_TEMP_VALUE)/(MAXIMUM_AD_VALUE-MINIMUM_AD_VALUE)+MINIMUM_TEMP_VALUE)

#define NUM_SAMPLES 1
#define TCY_NS      (1000000000ULL / FCY)

#define NUM_READINGS 10

#define MINIMUM_ADCS (120 / TCY_NS)
#define ADCS_SET     10


// Il +2 e' per stare larghi
#if ADCS_SET < (MINIMUM_ADCS + 2)
#error "Impostazione troppo veloce per il TAD dell'ADC"
#endif

static unsigned long temperature_average[NUM_READINGS]={0};
static int index=0;
static int first_loop=1;


void ptc_init(void) {
    // ASAM disabled; ADDMABM disabled; ADSIDL disabled; DONE disabled; SIMSAM Sequential; FORM Absolute decimal result,
    // unsigned, right-justified; SAMP disabled; SSRC Clearing sample bit ends sampling and starts conversion; AD12B
    // 12-bit; ADON enabled; SSRCG disabled;
    AD1CON1 = 0x0000;
    
    
    
    // CSCNA disabled; VCFG0 AVDD; VCFG1 AVSS; ALTS disabled; BUFM disabled; SMPI Generates interrupt after completion
    // of every sample/conversion operation; CHPS 1 Channel;
    AD1CON2 = 0x00;
    // SAMC 0; ADRC FOSC/2; ADCS 0;
    AD1CON3          = 0x0;
    AD1CON3bits.ADCS = ADCS_SET;
    // CH0SA AN0; CH0SB AN0; CH0NB AVSS; CH0NA AVSS;
    AD1CHS0 = 0x00;
    // CSS26 disabled; CSS25 disabled; CSS24 disabled; CSS31 disabled; CSS30 disabled;
    AD1CSSH = 0x00;
    // CSS2 disabled; CSS1 disabled; CSS0 disabled; CSS8 disabled; CSS7 disabled; CSS6 disabled; CSS5 disabled; CSS4
    // disabled; CSS3 disabled;
    AD1CSSL = 0x00;
    // DMABL Allocates 1 word of buffer to each analog input; ADDMAEN disabled;
    AD1CON4 = 0x00;
    // CH123SA disabled; CH123SB CH1=OA2/AN0,CH2=AN1,CH3=AN2; CH123NA disabled; CH123NB CH1=VREF-,CH2=VREF-,CH3=VREF-;
    //AD1CHS123      = 0x00;
    IFS0bits.AD1IF = 0;
}



// Routine che richiede fino a 50 microsecondi, da usare con parsimonia.
unsigned long ptc_read_input(int channel) {
    unsigned long value = 0, i, tmp, result;
    
    AD1CON1bits.ADON   = 0;
    AD1CON1bits.MODE12 = 1;
    AD1CHS0bits.CH0SA  = channel;
    AD1CON1bits.ADON   = 1;
    __delay_us(20);     // TODO: riduci
    
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        AD1CON1bits.SAMP = 1;
        __delay_us(2);     // Attesa di TAD*3 (due microsecondi sono abbondanti)
        AD1CON1bits.SAMP = 0;
        
        while (!IFS0bits.AD1IF)
        {
            __delay_us(1);
        }
        IFS0bits.AD1IF = 0;
        tmp            = ADC1BUF0;
        value += tmp;
    }
    AD1CON1bits.ADON = 0;
    
    result = value / NUM_SAMPLES;
    return result;
}



void ptc_read_temperature(void) {
    temperature_average[index] = ptc_read_input(PTC_CHANNEL);
    if (index==NUM_READINGS-1) 
        first_loop=0;
    index=(index+1)%NUM_READINGS;
}



uint16_t ptc_get_adc_value(void) {
    unsigned long temperature_sum=0;
    unsigned long temp=0;
    int i;
    int num_readings = first_loop ? index : NUM_READINGS;
    for (i=0; i<num_readings; i++) {
        temperature_sum+=temperature_average[i];
    }
    if (num_readings==0) {
        return 0;
    }
    else {
        temp = temperature_sum/num_readings;
        return (uint16_t)temp;
    }
}



int ptc_get_temperature(void) {
    uint16_t temp = ptc_get_adc_value();
    if (temp<=MINIMUM_AD_VALUE)
        return MINIMUM_TEMP_VALUE;
    else if (temp>=MAXIMUM_AD_VALUE)
        return MAXIMUM_TEMP_VALUE;
    else
        return (int) (COEFF_M_TIMES(temp)+COEFF_Q);
}
