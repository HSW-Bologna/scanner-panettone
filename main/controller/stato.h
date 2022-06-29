#ifndef STATO_H_INCLUDED
#define STATO_H_INCLUDED

#include "model/model.h"

typedef enum {
  STATO_EVENT_START,
  STATO_EVENT_STOP,
} stato_event_code_t;


void controller_stato_event(model_t *pmodel, stato_event_code_t event);
void controller_manage_stato(model_t *pmodel);


#endif