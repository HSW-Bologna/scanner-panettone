#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdint.h>

#include "lvgl.h"

#include "simulator_utils.h"
#include "display/monitor.h"
#include "view/view.h"
#include "controller/gui.h"
#include "controller/controller.h"
#include "controller/stato.h"
#include "simulator_utils.h"
#include "gel/keypad/keypad.h"
#include "gel/timer/timecheck.h"
#include "peripherals/keyboard.h"
#include "i2c_common/i2c_common.h"
#include "i2c_ports/posix/posix_eeprom_file_i2c_port.h"
#include "i2c_devices/eeprom/24LC16/24LC16.h"


int main(int argc, char *argv[]) {
    unsigned long tskp = 0;
    model_t       model;

    monitor_init();
    keyboard_init();

    model_init(&model);
    view_init(&model, monitor_flush, NULL, NULL, keyboard_reset);
    controller_init(&model);

    for (;;) {
        controller_manage_gui(&model);

        if (is_expired(tskp, get_millis(), 5)) {
            keypad_update_t update = keyboard_manage(get_millis());

            if (update.event != KEY_NOTHING) {
                view_event((view_event_t){.code = VIEW_EVENT_KEYPAD, .key_event = update});
            }
            tskp = get_millis();
        }

        SDL_Delay(1);
    }

    return 0;
}