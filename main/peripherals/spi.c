/*
 * File:   spi.c
 * Author: Virginia
 *
 * Created on 17 aprile 2021, 14.24
 */

#include <stdlib.h>
#include "peripherals/hardwareprofile.h"
#include "peripherals/system.h"
#include "peripherals/spi.h"

void spi_init(void) {
    SPI_MOSI_TRIS=TRIS_OUTPUT;
    SPI_CLK_TRIS=TRIS_OUTPUT;
    SPI_MISO_TRIS=TRIS_INPUT;
    SPI_CLK_LAT = 1;
    SPI_CS1_TRIS=TRIS_OUTPUT;
}

uint8_t spi1_master_exchange(uint8_t send, spi_speed_t speed){
    size_t  i;
    uint8_t val, valToSend;
    uint8_t byte = 0;

    for (i = 0; i < 8; i++) {
        valToSend = send & 0x80;
        SPI_MOSI_LAT = valToSend ? 1 : 0;
        SPI_CLK_LAT   = 0;
        __delay_us(speed == SPI_SPEED_SLOW ? 10 : 1);
        
        val      = SPI_MISO_PORT;
        byte = byte | val;

        if (i != 7)
        {
            byte = byte << 1;
            send = send << 1;
        }

        SPI_CLK_LAT = 1;
        __delay_us(speed == SPI_SPEED_SLOW ? 10 : 1);
    }
    SPI_MOSI_LAT = 0;
    return byte;
}