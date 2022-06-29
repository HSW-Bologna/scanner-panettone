#ifndef TEMPERATURE_H_INCLUDED
#define TEMPERATURE_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

int temperature_read(uint16_t *temperature, uint16_t *humidity);


#endif