#ifndef DIGOUT_H_INCLUDED
#define DIGOUT_H_INCLUDED

#include <string.h>
#include <stdint.h>

typedef enum {
    DIGOUT_ENABLE_SCANNER_MOTOR,
    DIGOUT_ENABLE_ROTATION_MOTOR,
    DIGOUT_DIRECTION_SCANNER_MOTOR,
    DIGOUT_DIRECTION_ROTATION_MOTOR,
    NUM_RELE,
} digout_t;

    //DIGOUT_ENABLE_SCANNER_MOTOR,
    //DIGOUT_DIRECTION_SCANNER_MOTOR,
    //DIGOUT_ENABLE_ROTATION_MOTOR,
    //DIGOUT_DIRECTION_ROTATION_MOTOR,

void digout_init(void);

void digout_update(digout_t rele, int val);
void clear_digout_all(void);

// #define toggle_digout_port(digout_t rele)  update_digout_port(out, !get_digout_port(out))
#define set_digout(rele)   digout_update(rele, 1)
#define clear_digout(rele) digout_update(rele, 0)



void    digout_buzzer_bip(size_t r, unsigned long t_on, unsigned long t_off);
void    digout_buzzer_check(void);
void    digout_buzzer_stop(void);
uint8_t digout_get_status(void);
uint8_t digout_get(digout_t);

#endif /* DIGOUT_H_INCLUDED */
