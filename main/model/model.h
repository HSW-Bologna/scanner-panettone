#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint16_t inputs;
    uint8_t state;
} model_t;


void model_init(model_t *pmodel);


#endif
