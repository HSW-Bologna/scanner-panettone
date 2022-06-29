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



enum {
    TIMER_BACK   = 0,
    TIMER_UPDATE = 1,
};


typedef enum {
    INDEX_TEMPO_CICLO = 0,
    INDEX_TEMPERATURA,
    INDEX_VELOCITA,
    INDEX_NUM,
} index_t;


static struct {
    lv_obj_t *label;
    lv_obj_t *value;
    index_t   current;

    lv_task_t *task;
    lv_task_t *time_task;
} page_data;


static void update_values(model_t *pmodel) {
    switch (page_data.current) {
        case INDEX_TEMPO_CICLO: {
            unsigned int secondi = model_get_stato_timer(pmodel);
            lv_label_set_text_fmt(page_data.value, "<%c>   %02im%02is",
                                  model_get_riscaldamento_attivo(pmodel) ? '*' : ' ', secondi / 60, secondi % 60);
            break;
        }

        case INDEX_TEMPERATURA:
            lv_label_set_text_fmt(page_data.value, "<%c> %03iC (%03iC)",
                                  model_get_riscaldamento_attivo(pmodel) ? '*' : ' ',
                                  model_temperatura_aria_ciclo(pmodel), pmodel->status.temperatura_rilevata);
            break;

        case INDEX_VELOCITA:
            lv_label_set_text_fmt(page_data.value, "%03i    g/min", model_velocita_ciclo(pmodel));
            break;

        case INDEX_NUM:
            break;
    }
}

static void update_all(model_t *pmodel) {
    switch (page_data.current) {
        case INDEX_TEMPO_CICLO:
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPO_CICLO));
            break;

        case INDEX_TEMPERATURA:
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_TEMPERATURA));
            break;

        case INDEX_VELOCITA:
            lv_label_set_text(page_data.label, view_intl_get_string(pmodel, STRINGS_VELOCITA));
            break;

        case INDEX_NUM:
            break;
    }

    update_values(pmodel);
}


static void *create_page(model_t *pmodel, void *extra) {
    return NULL;
}


static void open_page(model_t *pmodel, void *args) {
    (void)args;

    page_data.task      = view_register_periodic_task(5000UL, LV_TASK_PRIO_MID, TIMER_BACK);
    page_data.time_task = view_register_periodic_task(500UL, LV_TASK_PRIO_MID, TIMER_UPDATE);
    page_data.current   = INDEX_TEMPO_CICLO;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_label_set_body_draw(lbl, 1);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_set_width(lbl, LV_HOR_RES_MAX);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, -8);
    lv_obj_set_style(lbl, &style_label_8x16_reverse);
    page_data.label = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_set_width(lbl, LV_HOR_RES_MAX);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 8);
    lv_obj_set_style(lbl, &style_label_8x16);
    page_data.value = lbl;

    update_all(pmodel);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            switch (event.timer_id) {
                case TIMER_BACK:
                    msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                    break;

                case TIMER_UPDATE:
                    if (page_data.current == INDEX_TEMPO_CICLO || page_data.current == INDEX_TEMPERATURA) {
                        update_values(pmodel);
                    }
                    break;
            }
            break;

        case VIEW_EVENT_KEYPAD:
            if (event.key_event.event == KEY_CLICK || event.key_event.event == KEY_LONGPRESS) {
                lv_task_reset(page_data.task);

                switch (event.key_event.code) {
                    case BUTTON_STOP:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                        break;

                    case BUTTON_MENU:
                        if (page_data.current < INDEX_NUM - 1) {
                            page_data.current++;
                            update_all(pmodel);
                        } else {
                            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                        }
                        break;

                    case BUTTON_PLUS:
                        switch (page_data.current) {
                            case INDEX_TEMPO_CICLO:
                                model_modifica_durata_asciugatura(pmodel, 1);
                                break;

                            case INDEX_TEMPERATURA:
                                model_modifica_temperatura_aria(pmodel, 1);
                                break;

                            case INDEX_VELOCITA:
                                model_modifica_velocita(pmodel, 1);
                                break;

                            default:
                                break;
                        }
                        update_values(pmodel);
                        break;

                    case BUTTON_MINUS:
                        switch (page_data.current) {
                            case INDEX_TEMPO_CICLO:
                                model_modifica_durata_asciugatura(pmodel, -1);
                                break;

                            case INDEX_TEMPERATURA:
                                model_modifica_temperatura_aria(pmodel, -1);
                                break;

                            case INDEX_VELOCITA:
                                model_modifica_velocita(pmodel, -1);
                                break;

                            default:
                                break;
                        }
                        update_values(pmodel);
                        break;
                }
            }
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
    lv_task_del(page_data.time_task);
}


const pman_page_t page_adjust = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = close_page,
};
