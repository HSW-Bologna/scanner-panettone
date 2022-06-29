#include <stdio.h>
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "peripherals/digin.h"

static struct {
    lv_obj_t *digin_in;
    lv_obj_t *digin_status;
} page_data;

static void *create_page(model_t *model, void *extra) {
    return NULL;
}


static void open_page(model_t *model, void *data) {
    view_common_title(lv_scr_act(), "TEST INGRESSI");
    
    lv_obj_t *lblnum = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(lblnum, "7654321");
    lv_obj_align(lblnum, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 22);

    lv_obj_t *lblin = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lblin, 1);
    lv_obj_align(lblin, NULL, LV_ALIGN_IN_TOP_LEFT, 2, 32);
    page_data.digin_in = lblin;

    lv_obj_t *lblstato = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lblstato, 1);
    lv_obj_align(lblstato, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 32);
    page_data.digin_status = lblstato;
}


static view_message_t process_page_event(model_t *model, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_KEYPAD: {

            if (event.key_event.event == KEY_CLICK && event.key_event.code == BUTTON_CALDO) {
                msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_SWAP_PAGE;
                msg.vmsg.page = &page_digout_test;
            }
            if (event.key_event.event == KEY_CLICK && event.key_event.code == BUTTON_MEDIO) {
                msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_SWAP_PAGE;
                msg.vmsg.page = &page_coin_test;
            }
            if (event.key_event.event == KEY_CLICK && event.key_event.code == BUTTON_STOP) {
                msg.vmsg.code           = VIEW_PAGE_COMMAND_CODE_BACK;
                model->status.f_in_test = 0;
            }
            break;
        }
        case VIEW_EVENT_MODEL_UPDATE: {
            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_UPDATE;
            break;
        }

        default:
            break;
    }

    return msg;
}

static view_t update_page(model_t *model, void *arg) {

    lv_label_set_text_fmt(page_data.digin_in, "IN:", page_data.digin_in);
    lv_label_set_text_fmt(page_data.digin_status, "%07b", model->inputs);

    return 0;
}


const pman_page_t page_digin_test = {
    .create        = create_page,
    .open          = open_page,
    .update        = update_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
