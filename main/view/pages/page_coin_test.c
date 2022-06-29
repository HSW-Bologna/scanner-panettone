#include <stdio.h>
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "peripherals/digin.h"

static struct {
    unsigned int coins;
    lv_obj_t *   lbl_coins;
} page_data;


static void update_page(void) {
    lv_label_set_text_fmt(page_data.lbl_coins, "IMPULSI %8i", page_data.coins);
}


static void *create_page(model_t *model, void *extra) {
    return NULL;
}


static void open_page(model_t *model, void *data) {
    view_common_title(lv_scr_act(), "TEST GETT.");

    page_data.coins = 0;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 20);
    page_data.lbl_coins = lbl;

    update_page();
}


static view_message_t process_page_event(model_t *model, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_COIN:
            page_data.coins += event.coins;
            update_page();
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK && event.key_event.code == BUTTON_CALDO) {
                msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_SWAP_PAGE;
                msg.vmsg.page = &page_digin_test;
            }
            if (event.key_event.event == KEY_CLICK && event.key_event.code == BUTTON_MEDIO) {
                msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_SWAP_PAGE;
                msg.vmsg.page = &page_led_test;
            }
            if (event.key_event.event == KEY_CLICK && event.key_event.code == BUTTON_STOP) {
                page_data.coins = 0;
                update_page();
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


const pman_page_t page_coin_test = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
