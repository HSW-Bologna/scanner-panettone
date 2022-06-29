
#ifndef SPI_H_INCLUDED
#define	SPI_H_INCLUDED

#include <stdint.h>

typedef enum {
    SPI_SPEED_SLOW,
    SPI_SPEED_FAST,
} spi_speed_t ;

void spi_init(void);
uint8_t spi1_master_exchange(uint8_t send, spi_speed_t speed);

#endif	

