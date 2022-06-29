#include <stdio.h>
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "peripherals/led.h"

static const led_color_t colors[] = {
    LED_COLOR_BLACK,  LED_COLOR_RED,    LED_COLOR_GREEN,     LED_COLOR_BLUE,
    LED_COLOR_YELLOW, LED_COLOR_PURPLE, LED_COLOR_TURQUOISE, LED_COLOR_WHITE,
};

static struct {
    lv_obj_t *lbl_code;
    lv_obj_t *lbl_color;
    size_t    color;
} page_data;


static void update_page(void) {

    switch (colors[page_data.color]) {
        case LED_COLOR_BLACK:
            lv_label_set_text(page_data.lbl_color, "BLACK          0");
            lv_label_set_text(page_data.lbl_code, "[ 0 ]");
            break;

        case LED_COLOR_RED:
            lv_label_set_text(page_data.lbl_color, "R RED          1");
            lv_label_set_text(page_data.lbl_code, "[ 1 ]");
            break;

        case LED_COLOR_GREEN:
            lv_label_set_text(page_data.lbl_color, "G GREEN        2");
            lv_label_set_text(page_data.lbl_code, "[ 2 ]");
            break;

        case LED_COLOR_BLUE:
            lv_label_set_text(page_data.lbl_color, "B BLUE         3");
            lv_label_set_text(page_data.lbl_code, "[ 3 ]");
            break;

        case LED_COLOR_YELLOW:
            lv_label_set_text(page_data.lbl_color, "R+G YELLOW     4");
            lv_label_set_text(page_data.lbl_code, "[ 4 ]");
            break;

        case LED_COLOR_PURPLE:
            lv_label_set_text(page_data.lbl_color, "R+B VIOLET     5");
            lv_label_set_text(page_data.lbl_code, "[ 5 ]");
            break;

        case LED_COLOR_TURQUOISE:
            lv_label_set_text(page_data.lbl_color, "G+B TURQUOISE  6");
            lv_label_set_text(page_data.lbl_code, "[ 6 ]");
            break;

        case LED_COLOR_WHITE:
            lv_label_set_text(page_data.lbl_color, "R+G+B WHITE    7");
            lv_label_set_text(page_data.lbl_code, "[ 7 ]");
            break;

        default:
            break;
    }
}


static void *create_page(model_t *model, void *extra) {
    (void)extra;
    (void)model;
    return NULL;
}


static void open_page(model_t *model, void *data) {
    (void)data;

    page_data.color = 0;

    view_common_title(lv_scr_act(), "TEST LED");

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(lbl, "SET COLOR");
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 16);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 16);
    page_data.lbl_code = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
    page_data.lbl_color = lbl;

    update_page();
}

static view_message_t process_page_event(model_t *model, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {
                switch (event.key_event.code) {
                    case BUTTON_CALDO:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_SWAP_PAGE;
                        msg.vmsg.page = &page_coin_test;
                        break;

                    case BUTTON_MEDIO:
                        msg.vmsg.code  = VIEW_PAGE_COMMAND_CODE_SWAP_PAGE_EXTRA;
                        msg.vmsg.page  = &page_pwm_test;
                        msg.vmsg.extra = (void *)(uintptr_t)2;
                        break;

                    case BUTTON_STOP:
                        page_data.color = 0;
                        update_page();
                        msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_UPDATE_LED;
                        msg.cmsg.led  = colors[page_data.color];
                        break;

                    case BUTTON_MENU:
                        page_data.color = (page_data.color + 1) % 8;
                        update_page();
                        msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_UPDATE_LED;
                        msg.cmsg.led  = colors[page_data.color];
                        break;
                }
            }
        }

        case VIEW_EVENT_MODEL_UPDATE: {
            break;
        }

        default:
            break;
    }

    return msg;
}


const pman_page_t page_led_test = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
