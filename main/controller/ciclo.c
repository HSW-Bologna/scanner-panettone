#include <stdint.h>
#include <stdlib.h>
#include "ciclo.h"
#include "gel/state_machine/state_machine.h"
#include "gel/timer/timecheck.h"
#include "gel/timer/timer.h"
#include "model/model.h"
#include "peripherals/digin.h"
#include "peripherals/digout.h"
#include "peripherals/pwm.h"
#include "peripherals/timer.h"


#define SCANNER_SPEED_FAST   4800      // 10s x 1scan
#define SCANNER_SPEED_SCAN   400       // C1 - 120s x 1scan
#define MOVING_SPEED      10000
#define ROTATION_SPEED_FAST  12800     // 5s x 1rpm
#define ROTATION_SPEED_MEDIUM  8533      // C1 - 8rpm x 120s
#define ROTATION_SPEED_SLOW  3413      // C2 - 1rpm x / 30s

#define QUARTER_TURN_TIME 3333UL    // C2 ms x 1/4scan  #  10s/1scan

#define ENABLE_MOTOR 0
#define SCANNER_UP 0


typedef enum {
    CICLO_EVENT_START_1,
    CICLO_EVENT_START_2,
    CICLO_EVENT_LOWER_LIMIT,
    CICLO_EVENT_UPPER_LIMIT,
    CICLO_EVENT_ROTATION_LIMIT,
    CICLO_EVENT_STOP,
    CICLO_EVENT_RESET,
    CICLO_EVENT_TIMER,
} ciclo_event_t;


typedef enum {
    CICLO_STATE_OFF = 0,
    CICLO_STATE_CONTINUOUS_MODE_ASCENDING,
    CICLO_STATE_ROTATING,
    CICLO_STATE_MOVING,
    CICLO_STATE_RESETTING,
} ciclo_state_t;


DEFINE_STATE_MACHINE(ciclo, ciclo_event_t, model_t);


static int off_event_manager(model_t *pmodel, ciclo_event_t event);
static int continuous_mode_ascending_event_manager(model_t *pmodel, ciclo_event_t event);
static int rotating_event_manager(model_t *pmodel, ciclo_event_t event);
static int moving_event_manager(model_t *pmodel, ciclo_event_t event);
static int resetting_event_manager(model_t *pmodel, ciclo_event_t event);

static void rotate_stop(void);
static void scanner_stop(void);
static void event_timer(gel_timer_t *timer, void *arg1, void *arg2);
static int reset(void);


static size_t moving_step = 0;


static ciclo_event_manager_t managers[] = {
    [CICLO_STATE_OFF]                       = off_event_manager,
    [CICLO_STATE_CONTINUOUS_MODE_ASCENDING] = continuous_mode_ascending_event_manager,
    [CICLO_STATE_ROTATING]                  = rotating_event_manager,
    [CICLO_STATE_MOVING]                    = moving_event_manager,
    [CICLO_STATE_RESETTING]                 = resetting_event_manager,
};

static ciclo_state_machine_t sm = {
    .state    = CICLO_STATE_OFF,
    .managers = managers,
};


static gel_timer_t quarter_turn = {0};


void ciclo_init(void) {
    scanner_stop();
    rotate_stop();
}


int ciclo_manage_inputs(model_t *pmodel) {
    int res = 0;
        
    if (digin_get(DIGIN_START_CICLO_1)) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_START_1);
    }
    if (digin_get(DIGIN_START_CICLO_2)) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_START_2);
    }
    if (digin_get(DIGIN_STOP)) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_STOP);
    }
    if (digin_get(DIGIN_RESET)) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_RESET);
    }
    if (digin_get(DIGIN_MICRO_S_BASSO)) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_LOWER_LIMIT);
    }
    if (digin_get(DIGIN_MICRO_S_ALTO)) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_UPPER_LIMIT);
    }
    if (digin_read_pulses() > 0) {
        res |= ciclo_sm_send_event(&sm, pmodel, CICLO_EVENT_ROTATION_LIMIT);
        digin_clear_pulses();
    }
    
    pmodel->state = sm.state;
    
    return res;
}


void ciclo_manage_timers(model_t *pmodel) {
    gel_timer_manage_callbacks(&quarter_turn, 1, get_millis(), pmodel);
}


static void scanner_up(uint16_t frequency) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_S, frequency);
    digout_update(DIGOUT_ENABLE_SCANNER_MOTOR, ENABLE_MOTOR);
    digout_update(DIGOUT_DIRECTION_SCANNER_MOTOR, SCANNER_UP);
}


static void scanner_down(uint16_t frequency) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_S, frequency);
    digout_update(DIGOUT_ENABLE_SCANNER_MOTOR, ENABLE_MOTOR);
    digout_update(DIGOUT_DIRECTION_SCANNER_MOTOR, !SCANNER_UP);
}


static void scanner_stop(void) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_S, 0);
    digout_update(DIGOUT_ENABLE_SCANNER_MOTOR, !ENABLE_MOTOR);
}


static void scanner_hold(void) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_S, 0);
    digout_update(DIGOUT_ENABLE_SCANNER_MOTOR, ENABLE_MOTOR);
}


static void rotate_left(uint16_t frequency) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_P, frequency);
    digout_update(DIGOUT_ENABLE_ROTATION_MOTOR, ENABLE_MOTOR);
    digout_update(DIGOUT_DIRECTION_ROTATION_MOTOR, 1);
}


static void rotate_right(void) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_P, ROTATION_SPEED_FAST);
    digout_update(DIGOUT_ENABLE_ROTATION_MOTOR, ENABLE_MOTOR);
    digout_update(DIGOUT_DIRECTION_ROTATION_MOTOR, 0);
}


static void rotate_stop(void) {
    pwm_set(PWM_CHANNEL_OUT_STEP_MOTORE_P, 0);
    digout_update(DIGOUT_ENABLE_ROTATION_MOTOR, !ENABLE_MOTOR);
}


static uint8_t upper_limit_reached(void) {
    return digin_get(DIGIN_MICRO_S_ALTO) == 1;
}


static uint8_t lower_limit_reached(void) {
    if (digin_get(DIGIN_MICRO_S_BASSO)) {
        return  1;
    } else {
        return 0;
    }
}


static uint8_t rotation_zero_reached(void) {
    if (digin_get(DIGIN_MICRO_0_ROTATION)) {
        return  1;
    } else {
        return 0;
    }
}


static int off_event_manager(model_t *pmodel, ciclo_event_t event) {
    switch (event) {
        case CICLO_EVENT_START_1:
            if (lower_limit_reached()) {
                rotate_left(ROTATION_SPEED_MEDIUM);
                scanner_up(SCANNER_SPEED_SCAN);
                return CICLO_STATE_CONTINUOUS_MODE_ASCENDING;
            } else {
                digout_buzzer_bip(2, 200, 100);
                return -1;
            }
            
        case CICLO_EVENT_START_2:
            if (lower_limit_reached()) {
                moving_step = 0;
                rotate_left(ROTATION_SPEED_SLOW);
                scanner_hold();
                return CICLO_STATE_ROTATING;
            } else {
                digout_buzzer_bip(2, 200, 100);
                return -1;
            }
            
        case CICLO_EVENT_RESET:
            return reset();
            
        default:
            return -1;
    }
}


static int continuous_mode_ascending_event_manager(model_t *pmodel, ciclo_event_t event) {
    switch (event) {
        case CICLO_EVENT_STOP:
            rotate_stop();
            scanner_stop();
            return CICLO_STATE_OFF;

        case CICLO_EVENT_UPPER_LIMIT:
            scanner_down(SCANNER_SPEED_FAST);
            rotate_left(ROTATION_SPEED_FAST);
            return CICLO_STATE_RESETTING;

        case CICLO_EVENT_RESET:
            return reset();

        default:
            return -1;
    }
}


static int rotating_event_manager(model_t *pmodel, ciclo_event_t event) {
    switch (event) {
        case CICLO_EVENT_STOP:
            rotate_stop();
            scanner_stop();
            return CICLO_STATE_OFF;

        case CICLO_EVENT_RESET:
            return reset();
            
        case CICLO_EVENT_ROTATION_LIMIT:
            moving_step++;
            
            if (moving_step > 3 || upper_limit_reached()) {
                scanner_down(SCANNER_SPEED_FAST);
                rotate_left(ROTATION_SPEED_FAST);
                return CICLO_STATE_RESETTING;
            } else {
                rotate_stop();
                scanner_up(SCANNER_SPEED_FAST);
                gel_timer_activate(&quarter_turn, QUARTER_TURN_TIME, get_millis(), event_timer, (void*)CICLO_EVENT_TIMER);
                return CICLO_STATE_MOVING;
            }

        default:
            return -1;
    }
}


static int moving_event_manager(model_t *pmodel, ciclo_event_t event) {
    switch (event) {
        case CICLO_EVENT_STOP:
            rotate_stop();
            scanner_stop();
            return CICLO_STATE_OFF;

        case CICLO_EVENT_RESET:
            return reset();
            
        case CICLO_EVENT_UPPER_LIMIT:
        case CICLO_EVENT_TIMER: {
            rotate_left(ROTATION_SPEED_SLOW);
            scanner_hold();
            return CICLO_STATE_ROTATING;
        }
        
        default:
            return -1;
    }
}


static int resetting_event_manager(model_t *pmodel, ciclo_event_t event) {
    switch (event) {
        case CICLO_EVENT_UPPER_LIMIT:
        case CICLO_EVENT_LOWER_LIMIT:
            scanner_stop();
            if (digout_get(DIGOUT_ENABLE_ROTATION_MOTOR) == !ENABLE_MOTOR) {
                return CICLO_STATE_OFF;
            } else {
                return -1;
            }
            break;
            
        case CICLO_EVENT_STOP:
            rotate_stop();
            scanner_stop();
            return CICLO_STATE_OFF;

        case CICLO_EVENT_ROTATION_LIMIT:
            rotate_stop();
            if (digout_get(DIGOUT_ENABLE_SCANNER_MOTOR) == !ENABLE_MOTOR) {
                return CICLO_STATE_OFF;
            } else {
                return -1;
            }
            break;

        default:
            return -1;
    }
}


static void event_timer(gel_timer_t *timer, void *arg1, void *arg2) {
    model_t *pmodel = arg1;
    ciclo_sm_send_event(&sm, pmodel, (ciclo_event_t)arg2);
}


static int reset(void) {
    uint8_t res = 0;
    
    if (!lower_limit_reached()) {
        res = 1;
        scanner_down(SCANNER_SPEED_FAST);
    }
    if (!rotation_zero_reached()) {
        res = 1;
        rotate_left(ROTATION_SPEED_FAST);
    }
    return res ? CICLO_STATE_RESETTING : -1;
}