#ifndef CICLO_H_INCLUDED
#define CICLO_H_INCLUDED


#include "model/model.h"


typedef enum {
    CICLO_STATO_FERMO,
    CICLO_STATO_CONTINUO,
} ciclo_stato_t;


int ciclo_manage_inputs(model_t *pmodel);
void ciclo_init(void);


#endif