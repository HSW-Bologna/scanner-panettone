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


static struct {
    lv_task_t *task;
} page_data;


static void *create_page(model_t *pmodel, void *extra) {
    return NULL;
}


static void open_page(model_t *pmodel, void *args) {
    (void)args;

    page_data.task = view_register_periodic_task(100, LV_TASK_PRIO_HIGH, 0);
    //lv_task_ready(page_data.task);

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, LV_HOR_RES_MAX);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_SALVATAGGIO_IN_CORSO));
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_PARAMETERS_SAVE;
            break;

        case VIEW_EVENT_SAVED:
            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK_TWICE;
            break;

        default:
            break;
    }

    return msg;
}


static void close_page(void *data) {
    (void)data;
    lv_obj_clean(lv_scr_act());
    lv_task_del(page_data.task);
}


const pman_page_t page_wait = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = close_page,
};
