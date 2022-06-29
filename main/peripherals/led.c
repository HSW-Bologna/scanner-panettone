#include "SPI/spi_devices/io/MCP23S08/mcp23s08.h"
#include "hardwareprofile.h"
#include "spi.h"
#include "led.h"


static int spi_exchange(uint8_t *writebuf, uint8_t *readbuf, size_t length, void *data);
static int spi_cs_control(int level);

static spi_driver_t driver = {.spi_cs_control = spi_cs_control, .spi_exchange = spi_exchange, .user_data = NULL};


void led_init(void) {
    mcp23s08_init(driver, MCP23S08_DEFAULT_ADDR, 0);
    mcp23s08_set_gpio_direction_register(driver, MCP23S08_DEFAULT_ADDR, 0x00);
}


void led_set_color(led_color_t color) {
    mcp23s08_set_gpio_register(driver, MCP23S08_DEFAULT_ADDR, color);
}


static int spi_exchange(uint8_t *writebuf, uint8_t *readbuf, size_t length, void *data) {
    (void)data;

    size_t i = 0;
    for (i = 0; i < length; i++) {
        uint8_t to_write = 0;
        if (writebuf != NULL) {
            to_write = writebuf[i];
        }
        uint8_t response = spi1_master_exchange(to_write, SPI_SPEED_SLOW);
        if (readbuf != NULL) {
            readbuf[i] = response;
        }
    }

    return 0;
}


static int spi_cs_control(int level) {
    SPI_CS1_LAT = level;
    return 0;
}
