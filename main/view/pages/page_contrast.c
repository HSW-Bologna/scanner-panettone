#include <stdio.h>
#include "view/images/legacy.h"
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "view/widgets/custom_lv_img.h"
#include "view/intl/intl.h"
#include "peripherals/digin.h"


static struct page_data { lv_obj_t *lbl; } page_data;


static void update_page(model_t *pmodel) {
    lv_label_set_text_fmt(page_data.lbl, "[%02i]", pmodel->hsw.contrasto);
}


static void *create_page(model_t *model, void *extra) {
    return NULL;
}


static void open_page(model_t *pmodel, void *args) {
    (void)args;
    view_common_title(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_CONTRASTO_LCD));

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    page_data.lbl = lbl;
    update_page(pmodel);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {
                switch (event.key_event.code) {
                    case BUTTON_STOP:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                        msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_PRIVATE_PARAMETERS_SAVE;
                        break;

                    case BUTTON_TIEPIDO:
                        pmodel->hsw.contrasto = (pmodel->hsw.contrasto + 1) % 40;
                        msg.cmsg.code         = VIEW_CONTROLLER_COMMAND_CODE_UPDATE_CONTRAST;
                        update_page(pmodel);
                        break;

                    case BUTTON_FREDDO:
                        if (pmodel->hsw.contrasto > 10) {
                            pmodel->hsw.contrasto--;
                        } else {
                            pmodel->hsw.contrasto = 40;
                        }
                        msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_UPDATE_CONTRAST;
                        update_page(pmodel);
                        break;

                    case BUTTON_MENU:
                        pmodel->hsw.contrasto = 26;
                        msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_UPDATE_CONTRAST;
                        update_page(pmodel);
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


const pman_page_t page_contrast = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
