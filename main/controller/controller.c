#include "controller.h"
#include "view/view.h"
#include "model/model.h"



void controller_process_msg(view_controller_command_t *msg, model_t *pmodel) {
    switch (msg->code) {
        case VIEW_CONTROLLER_COMMAND_CODE_NOTHING:
            break;
    }
}


void controller_init(model_t *pmodel) {
    ciclo_init();
    view_change_page(pmodel, &page_main);
}