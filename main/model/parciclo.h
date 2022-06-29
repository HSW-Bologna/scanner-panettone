#ifndef PARCICLO_H_INCLUDED
#define PARCICLO_H_INCLUDED

#include "model.h"

void        parciclo_init(model_t *p, tipo_ciclo_t ciclo, int reset);
void        parciclo_operation(size_t parameter, int op, model_t *pmodel);
const char *parciclo_get_description(const model_t *pmodel, size_t parameter);
void        parciclo_format_value(char *string, size_t parameter, model_t *pmodel);
size_t      parciclo_get_tot_parameters(model_t *pmodel);

#endif