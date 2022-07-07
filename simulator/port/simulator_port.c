#include <stdint.h>
#include "peripherals/digout.h"
#include "peripherals/digin.h"
#include "peripherals/led.h"



int digin_get(digin_t digin) {
    return 0;
}

void Nop(void) {}

void digout_buzzer_bip(size_t r, unsigned long t_on, unsigned long t_off) {}

void digout_buzzer_stop(void) {}

void rele_set(digout_t rele, int val) {}

void pwm_set(uint8_t perc) {}

void digout_buzzer_set_off(void) {}

uint8_t digout_get_status(void) {
    return 0;
}


int uart_sync_write(uint8_t *data, int len) {
    return 0;
}


int uart_async_write(uint8_t *data, int len) {
    return 0;
}


int uart_read_rx_buffer(uint8_t *buffer) {
    return 0;
}


void uart_clean_rx_buffer() {}


void nt7534_reconfigure(uint8_t contrast) {
    (void)contrast;
}

void led_set_color(led_color_t led) {
    (void)led;
}

void pwoff_interrupt_enable(int i) {}

void pwoff_set_callback(void (*cb)()) {}

void digout_update(digout_t rele, int val) {}
