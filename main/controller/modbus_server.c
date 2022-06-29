#include <assert.h>
#include "libconf.h"
#include "modbus_server.h"
#include "peripherals/uart_driver.h"
#include "peripherals/digout.h"

#define SLAVE_ADDRESS 2

static ModbusSlave slave;
static uint8_t     coils[1] = {0};

ModbusError slaveExceptionCallback(const ModbusSlave *slave, uint8_t function, ModbusExceptionCode code);
ModbusError myRegisterCallback(const ModbusSlave *status, const ModbusRegisterCallbackArgs *args,
                               ModbusRegisterCallbackResult *result);
LIGHTMODBUS_WARN_UNUSED ModbusError modbusStaticAllocator(ModbusBuffer *buffer, uint16_t size, void *context);

void modbus_server_init(void) {
    ModbusErrorInfo err;
    err = modbusSlaveInit(&slave,
                          myRegisterCallback,                 // Callback for register operations
                          slaveExceptionCallback,             // Callback for handling slave exceptions (optional)
                          modbusStaticAllocator,              // Memory allocator for allocating responses
                          modbusSlaveDefaultFunctions,        // Set of supported functions
                          modbusSlaveDefaultFunctionCount     // Number of supported functions
    );

    // Check for errors
    assert(modbusIsOk(err) && "modbusSlaveInit() failed");
}

void modbus_server_manage(void) {
    uint8_t buffer[256];
    int     len = uart_read_rx_buffer(buffer);

    if (len == 0) {
        return;
    }

    ModbusErrorInfo err;
    err = modbusParseRequestRTU(&slave, SLAVE_ADDRESS, buffer, len);
    if (modbusIsOk(err)) {
        uart_async_write((uint8_t *)modbusSlaveGetResponse(&slave), modbusSlaveGetResponseLength(&slave));
        uart_clean_rx_buffer();
    }
}


ModbusError slaveExceptionCallback(const ModbusSlave *slave, uint8_t function, ModbusExceptionCode code) {
    // Always return MODBUS_OK
    return MODBUS_OK;
}

ModbusError myRegisterCallback(const ModbusSlave *status, const ModbusRegisterCallbackArgs *args,
                               ModbusRegisterCallbackResult *result) {
    switch (args->query) {

        case MODBUS_REGQ_R_CHECK:
        case MODBUS_REGQ_W_CHECK:

            result->exceptionCode = args->index < 1 ? MODBUS_EXCEP_NONE : MODBUS_EXCEP_ILLEGAL_ADDRESS;
            break;

        // Read register
        case MODBUS_REGQ_R:
            switch (args->type) {
                case MODBUS_COIL:
                    result->value = modbusMaskRead(coils, args->index);
                    break;
                default:
                    break;
            }
            break;

        // Write register
        case MODBUS_REGQ_W:
            switch (args->type) {
                case MODBUS_COIL:
                    modbusMaskWrite(coils, args->index, args->value);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    // Always return MODBUS_OK
    return MODBUS_OK;
}

LIGHTMODBUS_WARN_UNUSED ModbusError modbusStaticAllocator(ModbusBuffer *buffer, uint16_t size, void *context) {
    static uint8_t request[256];

    if (!size) {
        // Pretend we're freeing the buffer
        buffer->data = NULL;
        return MODBUS_OK;
    } else {
        if (size > 256) {
            // Requested size is too big, return allocation error
            buffer->data = NULL;
            return MODBUS_ERROR_ALLOC;
        } else {
            // Return a pointer to one of the static buffers depending
            // on the declared purpose of the buffer
            buffer->data = request;
            return MODBUS_OK;
        }
    }
}