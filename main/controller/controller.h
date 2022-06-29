#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "view/view_types.h"
#include "model/model.h"


void   controller_process_msg(view_controller_command_t *msg, model_t *model);
void   controller_init(model_t *pmodel);
void   controller_save_pars(model_t *pmodel);
void   controller_save_private_pars(model_t *pmodel);
size_t controller_update_pwoff(model_t *pmodel);
void   controller_save_pwoff(void);


#endif