/*
 * File:   digout.c
 * Author: Virginia
 *
 * Created on 17 aprile 2021, 12.12
 */

#include "digout.h"
#include "peripherals/hardwareprofile.h"
#include "peripherals/timer.h"
#include "gel/timer/timecheck.h"
#include "system.h"

static unsigned long time_on  = 0;
static unsigned long time_off = 0;
static size_t        repeat   = 0;
static int           is_set   = 0;
static unsigned long ts       = 0;


void digout_init(void) {
    RELE2_TRIS  = TRIS_OUTPUT;
    RELE3_TRIS  = TRIS_OUTPUT;
    RELE4_TRIS  = TRIS_OUTPUT;
    RELE5_TRIS  = TRIS_OUTPUT;
    BUZZER_TRIS = TRIS_OUTPUT;
}


void digout_update(digout_t rele, int val) {
    val = val > 0;
    switch (rele) {
        case DIGOUT_ENABLE_SCANNER_MOTOR:
            RELE3_LAT = val;
            break;
        case DIGOUT_ENABLE_ROTATION_MOTOR:
            RELE4_LAT = val;
            break;
        case DIGOUT_DIRECTION_SCANNER_MOTOR:
            RELE2_LAT = val;
            break;
        case DIGOUT_DIRECTION_ROTATION_MOTOR:
            RELE5_LAT = val;
            break;
        default:
            break;
    }
}

uint8_t digout_get_status(void) {
    uint8_t res = 0;
    res |= RELE2_LAT << 1;
    res |= RELE3_LAT << 2;
    res |= RELE4_LAT << 3;
    res |= RELE5_LAT << 4;
    return res;
}

uint8_t digout_get(digout_t rele) {
    switch (rele) {
        case DIGOUT_ENABLE_SCANNER_MOTOR:
            return RELE3_LAT;
        case DIGOUT_ENABLE_ROTATION_MOTOR:
            return RELE4_LAT;
        case DIGOUT_DIRECTION_SCANNER_MOTOR:
            return RELE2_LAT;
        case DIGOUT_DIRECTION_ROTATION_MOTOR:
            return RELE5_LAT;
        default:
            return 0;
    }
}



void digout_buzzer_bip(size_t r, unsigned long t_on, unsigned long t_off) {
    repeat     = r;
    time_on    = t_on;
    time_off   = t_off;
    is_set     = 1;
    BUZZER_LAT = 1;
    ts         = get_millis();
}


void digout_buzzer_stop(void) {
    BUZZER_LAT = 0;
    is_set     = 0;
    repeat     = 0;
    time_on    = 0;
    time_off   = 0;
}


void digout_buzzer_check(void) {
    if (is_set && repeat > 0) {
        if ((is_expired(ts, get_millis(), time_on)) && BUZZER_LAT) {
            BUZZER_LAT = !BUZZER_LAT;
            ts         = get_millis();
            repeat--;
        } else if ((is_expired(ts, get_millis(), time_off)) && !BUZZER_LAT) {
            BUZZER_LAT = !BUZZER_LAT;
            ts         = get_millis();
        }
    }

    if (is_set && repeat == 0) {
        is_set     = 0;
        ts         = 0;
        BUZZER_LAT = 0;
    }
}


void clear_digout_all(void) {
    RELE2_LAT = 0;
    RELE3_LAT = 0;
    RELE4_LAT = 0;
    RELE5_LAT = 0;
}
