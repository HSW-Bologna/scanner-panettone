#include <stdio.h>
#include <string.h>

#include "gel/pagemanager/page_manager.h"
#include "lvgl/lvgl.h"
#include "model/model.h"
#include "peripherals/keyboard.h"
#include "peripherals/pwm.h"
#include "view/styles.h"
#include "view/view.h"
#include "view/view_types.h"
#include "view/widgets/custom_lv_img.h"


enum {
    PERIODIC_TASK_ID,
};


static struct { 
    lv_task_t *task;
    
    lv_obj_t *lbl_pwm1;
    lv_obj_t *lbl_pwm2;

    pwm_channel_t channel;
} page_data;


static void *create_page(model_t *model, void *extra) {
    page_data.task = view_register_periodic_task(100, LV_TASK_PRIO_OFF, PERIODIC_TASK_ID);
    page_data.channel = PWM_CHANNEL_OUT_STEP_MOTORE_P;

    return NULL;
}


static void open_page(model_t *model, void *data) {
    lv_task_set_prio(page_data.task, LV_TASK_PRIO_MID);
    
    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_label_set_body_draw(lbl, 1);
    page_data.lbl_pwm1 = lbl;
    
    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 8);
    lv_label_set_body_draw(lbl, 1);
    page_data.lbl_pwm2 = lbl;
}


static view_message_t process_page_event(model_t *model, void *arg, pman_event_t event) {
    view_message_t msg = {.vmsg = {VIEW_PAGE_COMMAND_CODE_NOTHING}};

    switch (event.code) {

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {

                switch (event.key_event.code) {
                    case BUTTON_STOP_LANA:
                        break;

                    case BUTTON_STOP_FREDDO:
                        break;

                    case BUTTON_LINGUA:
                        break;

                    case BUTTON_CALDO:
                        break;

                    case BUTTON_MEDIO:
                        break;

                    case BUTTON_TIEPIDO:
                        break;

                    case BUTTON_LANA:
                        break;

                    case BUTTON_FREDDO:
                        break;

                    case BUTTON_MENU:
                        break;

                    case BUTTON_STOP:
                        break;

                    default:
                        break;
                }
            } else if (event.key_event.event == KEY_LONGCLICK) {
                switch (event.key_event.code) {
                    case BUTTON_STOP:
                        break;
                }
            } else if (event.key_event.event == KEY_PRESSING || event.key_event.event == KEY_LONGPRESS) {
                switch (event.key_event.code) {
                    case BUTTON_STOP:
                        break;
                }
            }
            break;
        }

        case VIEW_EVENT_CODE_TIMER:     // CADENZA 100msec
        {
            break;
        }

        default:
            break;
    }

    return msg;
}


static void update_page(void) {
    switch (page_data.channel) {
        case PWM_CHANNEL_OUT_STEP_MOTORE_P:
            lv_obj_set_style(page_data.lbl_pwm1, style_label_reverse);
            lv_obj_set_style(page_data.lbl_pwm2, style_label_normal);
            break;
            
        case PWM_CHANNEL_OUT_STEP_MOTORE_P:
            lv_obj_set_style(page_data.lbl_pwm2, style_label_reverse);
            lv_obj_set_style(page_data.lbl_pwm1, style_label_normal);
            break;
    }
}


static void page_destroy(void *data, void *extra) {
    lv_task_del(page_data.task);
}


static void page_close(void *data) {
    (void)data;
    lv_obj_clean(lv_scr_act());
    lv_task_set_prio(page_data.task, LV_TASK_PRIO_OFF);
}


const pman_page_t page_main = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = page_close,
    .destroy       = page_destroy,
};
