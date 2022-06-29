#include "i2c_ports/posix/posix_eeprom_file_i2c_port.h"
#include "i2c_devices/eeprom/24LC16/24LC16.h"

i2c_driver_t eeprom_driver = {.i2c_transfer = posix_eeprom_file_i2c_u8addr_port_transfer, .arg = "eeprom.bin"};