#include <stdio.h>
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "view/intl/intl.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"

static struct {
    size_t    index;
    lv_obj_t *cursors[NUM_CICLI];
} page_data;


static void *create_page(model_t *model, void *extra) {
    page_data.index = 0;
    return NULL;
}


static void open_page(model_t *model, void *data) {
    view_common_title(lv_scr_act(), view_intl_get_string(model, STRINGS_SCELTA_PROGRAMMA));
    lv_obj_t *labels[NUM_CICLI];

    size_t i = 0;
    for (i = 0; i < NUM_CICLI; i++) {
        lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
        labels[i]     = lbl;

        lbl = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_text(lbl, ">");
        page_data.cursors[i] = lbl;
    }

    strings_t program_names[NUM_CICLI] = {
        STRINGS_PROGRAMMA_CALDO,  STRINGS_PROGRAMMA_MEDIO, STRINGS_PROGRAMMA_TIEPIDO,
        STRINGS_PROGRAMMA_FREDDO, STRINGS_PROGRAMMA_LANA,
    };

    for (i = 0; i < NUM_CICLI; i++) {
        lv_label_set_text(labels[i], view_intl_get_string(model, program_names[i]));
    }

    lv_obj_align(labels[0], NULL, LV_ALIGN_IN_TOP_LEFT, 8, 20);
    lv_obj_align(labels[1], NULL, LV_ALIGN_IN_TOP_LEFT, 8, 35);
    lv_obj_align(labels[2], NULL, LV_ALIGN_IN_TOP_LEFT, LV_HOR_RES / 2, 20);
    lv_obj_align(labels[3], NULL, LV_ALIGN_IN_TOP_LEFT, LV_HOR_RES / 2, 35);
    lv_obj_align(labels[4], NULL, LV_ALIGN_IN_TOP_MID, 0, 50);

    for (i = 0; i < NUM_CICLI; i++) {
        lv_obj_align(page_data.cursors[i], labels[i], LV_ALIGN_OUT_LEFT_MID, 0, 0);
        lv_obj_set_hidden(page_data.cursors[i], 1);
    }
}

static view_message_t process_page_event(model_t *model, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {
                switch (event.key_event.code) {
                    case BUTTON_PLUS:
                        page_data.index = (page_data.index + 1) % NUM_CICLI;
                        msg.vmsg.code   = VIEW_PAGE_COMMAND_CODE_UPDATE;
                        break;

                    case BUTTON_MINUS:
                        if (page_data.index == 0) {
                            page_data.index = NUM_CICLI - 1;
                        } else {
                            page_data.index--;
                        }
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_UPDATE;
                        break;

                    case BUTTON_STOP:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                        break;

                    case BUTTON_LINGUA:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                        msg.vmsg.page = &page_parciclo;
                        msg.vmsg.extra = (void*)(uintptr_t)page_data.index;
                        break;

                    default:
                        break;
                }
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
    size_t previous, next;
    next = (page_data.index + 1) % NUM_CICLI;
    if (page_data.index == 0) {
        previous = NUM_CICLI - 1;
    } else {
        previous = page_data.index - 1;
    }

    lv_obj_set_hidden(page_data.cursors[previous], 1);
    lv_obj_set_hidden(page_data.cursors[page_data.index], 0);
    lv_obj_set_hidden(page_data.cursors[next], 1);

    return 0;
}


const pman_page_t page_scelta_programma = {
    .create        = create_page,
    .open          = open_page,
    .update        = update_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
