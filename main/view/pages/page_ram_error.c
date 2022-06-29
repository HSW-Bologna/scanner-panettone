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
#include "view/styles.h"
#include "view/intl/intl.h"


static int error_level = 0;


static void *create_page(model_t *pmodel, void *extra) {
    error_level = (int)(uintptr_t)extra;
    return NULL;
}


static void open_page(model_t *pmodel, void *args) {
    (void)args;

    view_common_title(lv_scr_act(), "ERR. EEPROM");

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style(lbl, &style_label_6x8);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, LV_HOR_RES_MAX);

    if (error_level == 1) {
        lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_ERRORE_FATALE_RAM));
    } else {
        lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_ERRORE_LIEVE_RAM));
    }

    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_LONGCLICK) {
                switch (event.key_event.code) {
                    case BUTTON_STOP:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_REBASE;
                        msg.vmsg.page = &page_splash;
                        break;

                    default:
                        break;
                }
            }
            break;
        }

        default:
            break;
    }

    return msg;
}


const pman_page_t page_ram_error = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
