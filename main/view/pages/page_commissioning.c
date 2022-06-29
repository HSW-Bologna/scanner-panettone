#include <stdio.h>
#include "view/images/legacy.h"
#include "view/styles.h"
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "view/widgets/custom_lv_img.h"
#include "peripherals/digin.h"
#include "view/intl/intl.h"
#include "model/parmac.h"


static struct page_data {
    lv_obj_t *lbl_lingua;
    lv_obj_t *lbl_codice_lingua;
    lv_obj_t *lbl_stringa_lingua;
    lv_obj_t *lbl_logo;
    lv_obj_t *lbl_codice_modello_macchina;
    lv_obj_t *lbl_modello_macchina;

    parmac_commissioning_t index;
} page_data;


static void update_strings(model_t *pmodel, int clear) {
    static parmac_commissioning_t index_old = PARMAC_COMMISSIONING_LINGUA;

    if (page_data.index == PARMAC_COMMISSIONING_LINGUA || index_old == PARMAC_COMMISSIONING_LINGUA || clear) {
        lv_label_set_text(page_data.lbl_lingua, parmac_commissioning_language_get_description(pmodel));
        lv_label_set_text_fmt(page_data.lbl_codice_lingua, "[%02i]", model_get_lingua(pmodel));

        const strings_t strings_lingua[] = {
            STRINGS_ITALIANO,
            STRINGS_INGLESE,
        };
        lv_label_set_text(page_data.lbl_stringa_lingua,
                          view_intl_get_string(pmodel, strings_lingua[model_get_lingua(pmodel)]));
    }

    if (page_data.index == PARMAC_COMMISSIONING_LOGO || index_old == PARMAC_COMMISSIONING_LOGO || clear) {
        size_t      logo           = model_get_logo_ditta(pmodel);
        const char *strings_logo[] = {
            "SMILE", "MSGroup", "RotondiGR", "HooverPRO", "Shulthess",
        };

        lv_label_set_text_fmt(page_data.lbl_logo, "LOGO[%i]%9s", logo, strings_logo[logo]);
    }

    if (page_data.index == PARMAC_COMMISSIONING_MODELLO || index_old == PARMAC_COMMISSIONING_MODELLO || clear) {
        lv_label_set_text_fmt(page_data.lbl_codice_modello_macchina, "MOD MAC [%02i]", model_get_machine_model(pmodel));
        lv_label_set_text(page_data.lbl_modello_macchina, view_common_modello_str(pmodel));
    }

    if (page_data.index != index_old || clear) {
        switch (page_data.index) {
            case PARMAC_COMMISSIONING_LINGUA:
                lv_obj_set_style(page_data.lbl_lingua, &style_label_reverse);
                lv_obj_set_style(page_data.lbl_logo, &style_label_normal);
                lv_obj_set_style(page_data.lbl_codice_modello_macchina, &style_label_normal);
                break;

            case PARMAC_COMMISSIONING_LOGO:
                lv_obj_set_style(page_data.lbl_lingua, &style_label_normal);
                lv_obj_set_style(page_data.lbl_logo, &style_label_reverse);
                lv_obj_set_style(page_data.lbl_codice_modello_macchina, &style_label_normal);
                break;

            case PARMAC_COMMISSIONING_MODELLO:
                lv_obj_set_style(page_data.lbl_lingua, &style_label_normal);
                lv_obj_set_style(page_data.lbl_logo, &style_label_normal);
                lv_obj_set_style(page_data.lbl_codice_modello_macchina, &style_label_reverse);
                break;

            default:
                break;
        }
    }

    index_old = page_data.index;
}


static void *create_page(model_t *model, void *extra) {
    parmac_setup_commissioning(model);
    page_data.index = PARMAC_COMMISSIONING_LINGUA;
    return NULL;
}


static void open_page(model_t *pmodel, void *arg) {
    (void)arg;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CROP);
    lv_label_set_body_draw(lbl, 1);
    lv_obj_set_width(lbl, LV_HOR_RES_MAX);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    page_data.lbl_lingua = lbl;

    lbl = lv_label_create(lv_scr_act(), page_data.lbl_lingua);
    lv_obj_align(lbl, page_data.lbl_lingua, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_data.lbl_codice_lingua = lbl;

    lbl = lv_label_create(lv_scr_act(), page_data.lbl_lingua);
    lv_obj_align(lbl, page_data.lbl_codice_lingua, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_data.lbl_stringa_lingua = lbl;

    lbl = lv_label_create(lv_scr_act(), page_data.lbl_lingua);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 4);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_LEFT);
    page_data.lbl_logo = lbl;

    lbl = lv_label_create(lv_scr_act(), page_data.lbl_lingua);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
    page_data.lbl_codice_modello_macchina = lbl;

    lbl = lv_label_create(lv_scr_act(), page_data.lbl_lingua);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    page_data.lbl_modello_macchina = lbl;

    update_strings(pmodel, 1);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_REBASE;
            msg.vmsg.page = &page_main;
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK) {
                switch (event.key_event.code) {
                    case BUTTON_CALDO:
                        page_data.index = (page_data.index + 1) % _NUM_PARMAC_COMMISSIONING;
                        update_strings(pmodel, 0);
                        break;

                    case BUTTON_MEDIO:
                        if (page_data.index > 0) {
                            page_data.index--;
                        } else {
                            page_data.index = _NUM_PARMAC_COMMISSIONING - 1;
                        }
                        update_strings(pmodel, 0);
                        break;

                    case BUTTON_TIEPIDO:
                        parmac_commissioning_operation(pmodel, page_data.index, +1);
                        update_strings(pmodel, 0);
                        break;

                    case BUTTON_FREDDO:
                        parmac_commissioning_operation(pmodel, page_data.index, -1);
                        update_strings(pmodel, 0);
                        break;

                    case BUTTON_LINGUA_TIEPIDO:
                        if (model_get_machine_model(pmodel) == MODELLO_MACCHINA_TEST) {
                            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_REBASE;
                            msg.vmsg.page = &page_main;
                        }
                        break;

                    case BUTTON_STOP:
                        if (model_get_machine_model(pmodel) != MODELLO_MACCHINA_TEST) {
                            model_init_parametri_ciclo(pmodel);
                            msg.cmsg.code = VIEW_CONTROLLER_COMMAND_CODE_PARAMETERS_SAVE;
                            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_REBASE;
                            msg.vmsg.page = &page_splash;
                        }
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


const pman_page_t page_commissioning = {
    .create        = create_page,
    .open          = open_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
