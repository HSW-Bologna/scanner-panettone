#include "view/view.h"
#include "gel/pagemanager/page_manager.h"
#include "model/model.h"
#include "peripherals/keyboard.h"
#include "lvgl/lvgl.h"
#include "view/fonts/legacy_fonts.h"
#include "view/common.h"
#include "view/styles.h"
#include "config/app_config.h"
#include <stdio.h>


#ifdef PC_SIMULATOR
const unsigned char versione_prg[] = "V:simulata";
#else
extern const unsigned char versione_prg[];
#endif


static struct {
    lv_obj_t * label_version;
    lv_obj_t * label_mem_free;
    lv_obj_t * label_mem_frag;
    lv_obj_t * label_low_watermark;
    lv_task_t *task;
} page_data;



static void *create_page(model_t *pmodel, void *extra) {
    page_data.task = view_common_register_timer(1000);
    return NULL;
}


static void open_page(model_t *pmodel, void *data) {
    lv_task_set_prio(page_data.task, LV_TASK_PRIO_MID);
    view_common_title(lv_scr_act(), "INFO");
    
    lv_obj_t *l_version = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(l_version, 1);
    lv_obj_align(l_version, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 15);
    lv_obj_set_style(l_version, &style_label_6x8);
    page_data.label_version = l_version;
    
    
    lv_obj_t *lbl_model = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style(lbl_model, &style_label_6x8);
    lv_label_set_text_fmt(lbl_model, "[%i] %s", model_get_machine_model(pmodel), view_common_modello_str(pmodel));
    lv_obj_align(lbl_model, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 24);

    
    lv_obj_t *mem_free = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(mem_free, 1);
    lv_obj_align(mem_free, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 35);
    page_data.label_mem_free = mem_free;
    
    lv_obj_t *mem_frag = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(mem_frag, 1);
    lv_obj_align(mem_frag, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 45);
    page_data.label_mem_frag = mem_frag;
    
    lv_obj_t *low_watermark = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(low_watermark, 1);
    lv_obj_align(low_watermark, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 55);
    page_data.label_low_watermark = low_watermark;
}


static view_message_t process_page_event(model_t *pmodel, void *arg, pman_event_t event) {
    view_message_t msg = {.vmsg = {VIEW_PAGE_COMMAND_CODE_NOTHING}};

    switch (event.code) {
        case VIEW_EVENT_CODE_TIMER:
            msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_UPDATE;
            break;

        case VIEW_EVENT_KEYPAD: {
            if (event.key_event.code == BUTTON_STOP && event.key_event.event == KEY_CLICK) {
                msg.vmsg.code = VIEW_PAGE_COMMAND_CODE_BACK;
            }
            break;
        }
        default:
            break;
    }

    return msg;
}


static view_t update_page(model_t *pmodel, void *arg) {
    lv_label_set_text(page_data.label_version, (char*)(versione_prg));
    lv_label_set_text_fmt(page_data.label_low_watermark, "WMARK: %i", (int)pmodel->lvgl_mem.low_watermark);
    lv_label_set_text_fmt(page_data.label_mem_frag, "FRAG %%: %i%%", (int)pmodel->lvgl_mem.frag_percentage);
    lv_label_set_text_fmt(page_data.label_mem_free, "USED %%: %i%%", (int)pmodel->lvgl_mem.used_percentage);

    return 0;
}


static void page_close(void *data) {
    (void)data;
    lv_obj_clean(lv_scr_act());
    lv_task_set_prio(page_data.task, LV_TASK_PRIO_OFF);
}


static void page_destroy(void *data, void *extra) {
    lv_task_del(page_data.task);
}


const pman_page_t page_info = {
    .create        = create_page,
    .open          = open_page,
    .update        = update_page,
    .process_event = process_page_event,
    .close         = page_close,
    .destroy       = page_destroy,
};