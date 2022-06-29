#include <stdio.h>
#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "model/parmac.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "peripherals/digin.h"

static struct {
    lv_obj_t *lnum;
    lv_obj_t *ldesc;
    lv_obj_t *lval;

    size_t parameter;
    size_t num_parameters;
    size_t livello_accesso;
    int    par_to_save;

} page_data;


static void *create_page(model_t *model, void *extra) {
    parmac_setup_full(model, 0, 0);
    page_data.par_to_save     = 0;
    page_data.livello_accesso = model->pmac.abilita_parametri_ridotti;
    return NULL;
}


static void open_page(model_t *model, void *data) {
    view_common_title(lv_scr_act(), "PARAMETRI MAC.");

    page_data.num_parameters = parmac_get_tot_parameters(page_data.livello_accesso);
    page_data.parameter      = 0;

    lv_obj_t *lnum = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(lnum, 1);
    lv_obj_align(lnum, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    page_data.lnum = lnum;

    lv_obj_t *ldesc = lv_label_create(lv_scr_act(), lnum);
    lv_obj_align(ldesc, lnum, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    page_data.ldesc = ldesc;

    lv_obj_t *lval = lv_label_create(lv_scr_act(), lnum);
    lv_obj_align(lval, ldesc, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    page_data.lval = lval;
}


static view_message_t process_page_event(model_t *model, void *arg, pman_event_t event) {
    view_message_t msg = VIEW_EMPTY_MSG;

    switch (event.code) {
        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.event == KEY_CLICK || event.key_event.event == KEY_LONGPRESS) {
                switch (event.key_event.code) {
                    case BUTTON_MEDIO:
                        if (page_data.parameter > 0) {
                            page_data.parameter--;
                        } else {
                            page_data.parameter = page_data.num_parameters - 1;
                        }
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_UPDATE;
                        break;

                    case BUTTON_CALDO:
                        page_data.parameter = (page_data.parameter + 1) % page_data.num_parameters;
                        msg.vmsg.code       = VIEW_PAGE_COMMAND_CODE_UPDATE;
                        break;

                    case BUTTON_MINUS:
                        parmac_operation(model, page_data.parameter, -1, page_data.livello_accesso);
                        msg.vmsg.code         = VIEW_PAGE_COMMAND_CODE_UPDATE;
                        page_data.par_to_save = 1;
                        break;

                    case BUTTON_PLUS:
                        parmac_operation(model, page_data.parameter, +1, page_data.livello_accesso);
                        msg.vmsg.code         = VIEW_PAGE_COMMAND_CODE_UPDATE;
                        page_data.par_to_save = 1;
                        break;

                    case BUTTON_STOP:
                        msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
#warning "Rimuovere i parametri estesi in uscita dalla pagina!" // -TODO !!!!
                        // model->pmac.abilita_parametri_ridotti = 1;
                        if (page_data.par_to_save) {
                            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_wait;
                        } else {
                            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
                        }
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

static view_t update_page(model_t *pmodel, void *arg) {
    char string[64] = {0};

    lv_label_set_text_fmt(page_data.lnum, "Param. %2i/%i", page_data.parameter + 1, page_data.num_parameters);
    lv_label_set_text(page_data.ldesc, parmac_get_description(pmodel, page_data.parameter, page_data.livello_accesso));
    parmac_format_value(pmodel, string, page_data.parameter, page_data.livello_accesso);
    lv_label_set_text(page_data.lval, string);
    return 0;
}


const pman_page_t page_parmac = {
    .create        = create_page,
    .open          = open_page,
    .update        = update_page,
    .process_event = process_page_event,
    .close         = view_close_all,
};
