/*
 * File:   gettoniera.c
 * Author: Virginia
 *
 * Created on 8 maggio 2021, 12.09
 */

#include "gettoniera.h"
#include "hardwareprofile.h"
#include "gel/debounce/pulsecounter.h"

static pulse_filter_t filter;

// GETTONIERA DIGITALE
void gettoniera_init(void) {
    GETT_TRIS=TRIS_INPUT;
    ANSBbits.ANSELB13 = 0;
    pulse_filter_init(&filter, COUNT_LOW_PULSE, 1);
}

void gettoniera_reset_count(void) {
    pulse_clear(&filter, 0);
    pulse_clear(&filter, 1);
}

int gettoniera_take_insert(void) {
    unsigned int input = (GETT_PORT | (!IN3_PORT << 1)); // SAREBBE " GETTONE1 "
    
    return pulse_filter(&filter, input, 10);
}

unsigned int gettoniera_get_count(void) {
    return pulse_count(&filter,0);
}


unsigned int gettoniera_get_count_ingresso(void) {
    return pulse_count(&filter, 1);
}

int gettoniera_get_pulse_level(void) {
    return pulse_level(&filter, 0);
}
