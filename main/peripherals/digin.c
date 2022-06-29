/*
 * File:   digin.c
 * Author: Virginia
 *
 * Created on 20 aprile 2021, 12.04
 */


#include "digin.h"
#include "hardwareprofile.h"
#include "gel/debounce/debounce.h"

static debounce_filter_t filter;

void digin_init(void) {
    IN1_TRIS=TRIS_INPUT;
    IN2_TRIS=TRIS_INPUT;
    IN3_TRIS=TRIS_INPUT;
    IN4_TRIS=TRIS_INPUT;
    IN5_TRIS=TRIS_INPUT;
    IN6_TRIS=TRIS_INPUT;
    IN7_TRIS=TRIS_INPUT;
    
    debounce_filter_init(&filter);
}

int digin_get(digin_t digin) {
   return debounce_read(&filter, digin);
}

int digin_take_reading(void) {
    unsigned int input=0;
    input|=!IN1_PORT;
    input|=(!IN2_PORT)<<1;
    input|=(!IN3_PORT)<<2;
    input|=(!IN4_PORT)<<3;
    input|=(!IN5_PORT)<<4;
    input|=(!IN6_PORT)<<5;
    input|=(!IN7_PORT)<<6;
    return debounce_filter(&filter, input, 10);
}

unsigned int digin_get_inputs(void) {
    return debounce_value(&filter);
}
