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


static struct page_data {
    lv_task_t *task;
    lv_obj_t * label;
    lv_obj_t * lbl_stat;
    size_t     index;
} page_data;


static void update_page(model_t *pmodel) {
    char string[64] = {0};

    switch (page_data.index) {
        case 0: {
            unsigned long time = pmodel->pwoff.tempo_attivita;
            sprintf(string, "%06luh %02lum %02lus", time / 3600, (time / 60) % 60, time % 60);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPO_ACCENSIONE));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 1: {
            unsigned long time = pmodel->pwoff.tempo_lavoro;
            sprintf(string, "%06luh %02lum %02lus", time / 3600, (time / 60) % 60, time % 60);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPO_LAVORO));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 2: {
            unsigned long time = pmodel->pwoff.tempo_moto;
            sprintf(string, "%06luh %02lum %02lus", time / 3600, (time / 60) % 60, time % 60);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPO_MOTO));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 3: {
            unsigned long time = pmodel->pwoff.tempo_ventilazione;
            sprintf(string, "%06luh %02lum %02lus", time / 3600, (time / 60) % 60, time % 60);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPO_VENTILAZIONE));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 4: {
            unsigned long time = pmodel->pwoff.tempo_riscaldamento;
            sprintf(string, "%06luh %02lum %02lus", time / 3600, (time / 60) % 60, time % 60);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPO_RISCALDAMENTO));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 5: {
            sprintf(string, "%06u", pmodel->pwoff.cicli_parziali);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_CICLI_PARZIALI));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 6: {
            sprintf(string, "%06u", pmodel->pwoff.cicli_totali);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_CICLI_TOTALI));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        case 7: {
            sprintf(string, "%06u", pmodel->pwoff.gettoni);
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_GETTONI_TOTALI));
            lv_label_set_text(page_data.lbl_stat, string);
            break;
        }

        default:
            break;
    }
}


static void *create_page(model_t *model, void *extra) {
    return NULL;
}


static void open_page(model_t *pmodel, void *args) {
    (void)args;

    page_data.index = 0;

    view_common_title(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_STATISTICHE));
    page_data.task = view_register_periodic_task(500, LV_TASK_PRIO_HIGH, 0);

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    page_data.label = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(lbl, page_data.label, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    page_data.lbl_stat = lbl;

    update_page(pmodel);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            update_page(pmodel);
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {
                switch (event.key_event.code) {
                    case BUTTON_STOP:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                        break;

                    case BUTTON_CALDO:
                        page_data.index = (page_data.index + 1) % 8;
                        update_page(pmodel);
                        break;

                    case BUTTON_MEDIO:
                        if (page_data.index > 0) {
                            page_data.index--;
                        } else {
                            page_data.index = 7;
                        }
                        update_page(pmodel);
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


static void close_page(void *data) {
    (void)data;
    lv_obj_clean(lv_scr_act());
    lv_task_del(page_data.task);
}

const pman_page_t page_stats = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = close_page,
};
