#include "lvgl/lvgl.h"
#include "controller.h"
#include "view/view.h"
#include "gel/timer/timecheck.h"
#include "peripherals/digout.h"
#include "peripherals/timer.h"
#include "peripherals/NT7534.h"
#include "gui.h"


void controller_manage_gui(model_t *model) {
    static unsigned long last_invoked = 0;
    static unsigned long ts_refresh = 0;

    view_message_t umsg;
    view_event_t   event;
    lv_mem_monitor_t mem_monitor;

    lv_task_handler();
    
    if (last_invoked != get_millis()) {
        lv_tick_inc(time_interval(last_invoked, get_millis()));
        last_invoked = get_millis();
    }

    while (view_get_next_msg(model, &umsg, &event)) {
        controller_process_msg(&umsg.cmsg, model);
        view_process_msg(umsg.vmsg, model);

        if (event.code == VIEW_EVENT_KEYPAD && event.key_event.event == KEY_CLICK) {
            digout_buzzer_bip(1, 100, 0);
        }
    }
    
    if (is_expired(ts_refresh, get_millis(), 20000UL)) {
        nt7534_reconfigure(26);
        lv_obj_invalidate(lv_scr_act());
        ts_refresh = get_millis();
    }
}
