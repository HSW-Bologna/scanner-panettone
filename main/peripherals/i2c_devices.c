/*
 * File:   i2c_devices.c
 * Author: Virginia
 *
 * Created on 21 aprile 2021, 11.37
 */

#include "i2c_common/i2c_common.h"
#include "i2c_ports/PIC/i2c_bitbang.h"
#include "i2c_devices/eeprom/24LC16/24LC16.h"
#include "i2c_ports/PIC/i2c_driver.h"

i2c_driver_t eeprom_driver= {.device_address=EEPROM24LC16_DEFAULT_ADDRESS, .i2c_transfer=pic_i2c_bitbang_port_transfer, .ack_polling=pic_i2c_bitbang_port_ack_polling};
