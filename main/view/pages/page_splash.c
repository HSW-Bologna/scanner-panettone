#include <stdio.h>
#include "view/images/legacy.h"
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "view/widgets/custom_lv_img.h"
#include "peripherals/digin.h"



static void *create_page(model_t *model, void *extra) {
    return NULL;
}


static void open_page(model_t *pmodel, void *data) {
    (void)data;

#ifdef PC_SIMULATOR
    lv_task_t *task = view_register_periodic_task(1000, LV_TASK_PRIO_HIGH, 3);
#else
    lv_task_t *task = view_register_periodic_task(5000, LV_TASK_PRIO_HIGH, 3);
#endif
    lv_task_once(task);

    lv_obj_t *logo = custom_lv_img_create(lv_scr_act(), NULL);

    switch (model_get_logo_ditta(pmodel)) {
        case 0:
            custom_lv_img_set_src(logo, &legacy_img_logo_ciao);
            break;

        case 1:
            custom_lv_img_set_src(logo, &legacy_img_logo_msgroup);
            break;

        case 2:
            custom_lv_img_set_src(logo, &legacy_img_logo_rotondi);
            break;

        case 3:
            custom_lv_img_set_src(logo, &legacy_img_logo_hoover);
            break;

        case 4:
            custom_lv_img_set_src(logo, &legacy_img_logo_schulthess);
            break;

        default:
            break;
    }

    lv_obj_align(logo, NULL, LV_ALIGN_CENTER, 0, 0);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_REBASE;
            if (model_is_machine_selected(pmodel)) {
                msg.vmsg.page = &page_main;
            } else {
                msg.vmsg.page = &page_commissioning;
            }
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {
                switch (event.key_event.code) {
                    case BUTTON_STOP_LANA:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_REBASE;
                        msg.vmsg.page = &page_reset_ram;
                        break;
                }
            }
            break;
        }

        case VIEW_EVENT_MODEL_UPDATE: {
            break;
        }

        default:
            break;
    }

    return msg;
}


const pman_page_t page_splash = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
