#include "peripherals/system.h"
#include "peripherals/hardwareprofile.h"
#include "lvgl/lvgl.h"
#include "peripherals/NT7534.h"
#include "peripherals/digout.h"
#include "peripherals/keyboard.h"
#include "peripherals/digin.h"
#include "peripherals/i2c_devices.h"
#include "i2c_ports/PIC/i2c_driver.h"
#include "i2c_ports/PIC/i2c_bitbang.h"
#include "i2c_common/i2c_common.h"
#include "peripherals/timer.h"
#include "peripherals/spi.h"
#include "gel/timer/timecheck.h"
#include "model/model.h"
#include "view/view.h"
#include "controller/gui.h"
#include "peripherals/pwm.h"
#include "controller/controller.h"
#include "controller/ciclo.h"
#include "peripherals/uart_driver.h"

model_t model;



int main(void) {
    unsigned long tskp = 0, ts_input = 0;
    
    // inizializzazioni ----------------------- //
    system_init();
    
    spi_init();
    nt7534_init();
    digout_init();
    keyboard_init();
    
    i2c_bitbang_init(2);
    digin_init();
    timer_init();
    pwm_init();
    
    uart_init();
    
    clear_digout_all();
    
    model_init(&model);
    view_init(&model, nt7534_flush, nt7534_rounder, nt7534_set_px, keyboard_reset);
    
    controller_init(&model);
    
    digout_buzzer_bip(2, 100, 100);

    // MAIN LOOP ============================================================ //
    for (;;) {
        controller_manage_gui(&model);
        
        ClrWdt();
        
        if (is_expired(ts_input, get_millis(), 2)) {
            if (digin_take_reading()) {
                model.inputs = digin_get_inputs();
                ciclo_manage_inputs(&model);
                view_event((view_event_t){.code = VIEW_EVENT_MODEL_UPDATE});
            }
            
            ts_input = get_millis();
        }
        
        if (is_expired(tskp, get_millis(), 5)) {
            keypad_update_t update = keyboard_manage(get_millis());

            if (update.event != KEY_NOTHING) {
                view_event((view_event_t){.code = VIEW_EVENT_KEYPAD, .key_event = update});
            }
            tskp = get_millis();
        }
        
        // controllo buzzer
        digout_buzzer_check();

        __delay_us(10);
    }
    return 0;
}
