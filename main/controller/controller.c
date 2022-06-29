#include "controller.h"

#include "controller/stato.h"
#include "view/view_types.h"
#include "model/model.h"
#include "model/parmac.h"
#include "model/parciclo.h"
#include "peripherals/pwm.h"
#include "peripherals/digout.h"
#include "peripherals/led.h"
#include "view/view.h"
#include "peripherals/i2c_devices.h"
#include "peripherals/NT7534.h"
#include "i2c_devices/eeprom/24LC16/24LC16.h"
#include "controller.h"
#include "gel/serializer/serializer.h"
#include "peripherals/pwoff.h"
#include "gel/wearleveling/wearleveling.h"

#define CHECK_BYTE           0xAA
#define CHECK_BYTE_ADDRESS   0
#define PAR_START_ADDRESS    (CHECK_BYTE_ADDRESS) + 1
#define PRIVATE_PARS_ADDRESS 256UL

#define PWOFF_DATA_ADDRESS (PRIVATE_PARS_ADDRESS + 128UL)

static uint8_t pwoff_data[PWOFF_SERIALIZED_SIZE] = {0};
#ifdef ENABLE_WEARLEVELING

#if (PWOFF_DATA_ADDRESS % 16) != 0
#error "Wear leveled memory should be aligned to page size!"
#endif

#define WL_BLOCK_SIZE                32UL
#define WL_MEMORY_SIZE               256
#define WL_BLOCK_NUM                 8
#define WL_MARKER_ADDRESS(block_num) ((PWOFF_DATA_ADDRESS) + ((block_num) * (WL_BLOCK_SIZE)))
#define WL_DATA_ADDRESS(block_num)   ((PWOFF_DATA_ADDRESS) + ((block_num) * (WL_BLOCK_SIZE) + 1))

#if PWOFF_SERIALIZED_SIZE > WL_BLOCK_SIZE
#error "Wear leveled blocks are too small!"
#endif

static wear_leveled_memory_t memory;

static int read_marker(size_t block_num, uint8_t *marker);
static int read_block(size_t block_num, uint8_t *buffer, size_t len);
static int write_block(size_t block_num, uint8_t marker, uint8_t *buffer, size_t len);
#endif

static int controller_start_check(void);



void controller_process_msg(view_controller_command_t *msg, model_t *pmodel) {
    switch (msg->code) {
        case VIEW_CONTROLLER_COMMAND_CODE_UPDATE_PWM: {
            if (msg->output == 1)
                pmodel->pwm1 = msg->value;
            if (msg->output == 2)
                pmodel->pwm2 = msg->value;
            break;
        }

        case VIEW_CONTROLLER_COMMAND_CODE_UPDATE_DIGOUT: {
            rele_update(msg->output - 1, msg->value);
            int i = 0;
            for (i = 0; i < 6; i++) {
                if (i != (msg->output) - 1)
                    rele_update(i, 0);
            }
            pmodel->outputs = rele_get_status();

            view_event((view_event_t){.code = VIEW_EVENT_MODEL_UPDATE});
            break;
        }

        case VIEW_CONTROLLER_COMMAND_CODE_DIGOUT_TURNOFF: {
            int i = 0;
            for (i = 0; i < 6; i++) {
                rele_update(i, 0);
                pmodel->outputs = rele_get_status();
            }

            view_event((view_event_t){.code = VIEW_EVENT_MODEL_UPDATE});
            break;
        }

        case VIEW_CONTROLLER_COMMAND_CODE_PARAMETERS_SAVE: {
            digout_buzzer_stop();
            controller_save_pars(pmodel);
            view_event((view_event_t){.code = VIEW_EVENT_SAVED});
            break;
        }

        case VIEW_CONTROLLER_COMMAND_CODE_RESET_RAM: {
            model_reset_parameters(pmodel);
            model_init_parametri_ciclo(pmodel);
            parmac_init(pmodel, 1);
            controller_save_pars(pmodel);
            break;
        }

        case VIEW_CONTROLLER_COMMAND_CODE_UPDATE_LED: {
            led_set_color(msg->led);
            break;
        }

        case VIEW_CONTROLLER_COMMAND_CODE_UPDATE_CONTRAST:
            nt7534_reconfigure(pmodel->hsw.contrasto);
            break;

        case VIEW_CONTROLLER_COMMAND_CODE_PRIVATE_PARAMETERS_SAVE:
            controller_save_private_pars(pmodel);
            break;

        case VIEW_CONTROLLER_COMMAND_CODE_NOTHING:
            break;
    }
}


void controller_init(model_t *pmodel)
{
#ifdef ENABLE_WEARLEVELING
    wearleveling_init(&memory, read_block, write_block, read_marker, WL_BLOCK_NUM);
#endif
    size_t i = 0;

    if (!controller_start_check())
    {
        uint8_t data[PARS_SERIALIZED_SIZE] = {0};
        parmac_init(pmodel, 1);
        
        for (i = 0; i < NUM_CICLI; i++)
        {
            parciclo_init(pmodel, i, 1);
        }
        uint8_t check_byte = CHECK_BYTE;
        EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, CHECK_BYTE_ADDRESS, &check_byte, 1);

        i = model_pars_serialize(pmodel, data);
        EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, PAR_START_ADDRESS, data, i);

        i = model_private_parameters_serialize(pmodel, data);
        EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, PRIVATE_PARS_ADDRESS, data, i);
    }
    else
    {
        uint8_t data[PARS_SERIALIZED_SIZE] = {0};
        EE24CL16_SEQUENTIAL_READ(eeprom_driver, PAR_START_ADDRESS, data, PARS_SERIALIZED_SIZE);
        model_pars_deserialize(pmodel, data);

        EE24CL16_SEQUENTIAL_READ(eeprom_driver, PRIVATE_PARS_ADDRESS, data, PRIVATE_PARS_SERIALIZED_SIZE);
        model_private_parameters_deserialize(pmodel, data);

        parmac_init(pmodel, 0);
        
        for (i = 0; i < NUM_CICLI; i++)
        {
            parciclo_init(pmodel, i, 0);
        }

        uint8_t pwoff_data[PWOFF_SERIALIZED_SIZE] = {0};
#ifdef ENABLE_WEARLEVELING
        wearleveling_read(&memory, pwoff_data, PWOFF_SERIALIZED_SIZE);
#else
        EE24CL16_SEQUENTIAL_READ(eeprom_driver, PWOFF_DATA_ADDRESS, pwoff_data, PWOFF_SERIALIZED_SIZE);
#endif
        model_pwoff_deserialize(pmodel, pwoff_data);
        controller_update_pwoff(pmodel);
    }
    
    pwoff_set_callback(controller_save_pwoff);
    nt7534_reconfigure(pmodel->hsw.contrasto);
    view_change_page(pmodel, &page_splash);
    
    
    
    
}


void controller_save_pars(model_t *pmodel) {
    uint8_t data[PARS_SERIALIZED_SIZE] = {0};
    size_t  i                          = model_pars_serialize(pmodel, data);
    EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, PAR_START_ADDRESS, data, i);
}


void controller_save_private_pars(model_t *pmodel) {
    uint8_t data[PRIVATE_PARS_SERIALIZED_SIZE] = {0};
    size_t  i                                  = model_private_parameters_serialize(pmodel, data);
    EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, PRIVATE_PARS_ADDRESS, data, i);
}


size_t controller_update_pwoff(model_t *pmodel) {
    pwoff_interrupt_enable(0);
    size_t i = model_pwoff_serialize(pmodel, pwoff_data);
    pwoff_interrupt_enable(1);
    return i;
}


void controller_save_pwoff(void) {
#ifdef ENABLE_WEARLEVELING
    wearleveling_write(&memory, pwoff_data, PWOFF_SERIALIZED_SIZE);
#else
    EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, PWOFF_DATA_ADDRESS, pwoff_data, PWOFF_SERIALIZED_SIZE);
#endif
}


#if ENABLE_WEARLEVELING
static int read_marker(size_t block_num, uint8_t *marker) {
    if (block_num > (WL_BLOCK_NUM - 1)) {
        return 1;
    }

    EE24CL16_SEQUENTIAL_READ(eeprom_driver, WL_MARKER_ADDRESS(block_num), marker, 1);
    return 0;
}

static int read_block(size_t block_num, uint8_t *buffer, size_t len) {
    if (block_num > (WL_BLOCK_NUM - 1)) {
        return 1;
    }
    EE24CL16_SEQUENTIAL_READ(eeprom_driver, WL_DATA_ADDRESS(block_num), buffer, len);
    return 0;
}

static int write_block(size_t block_num, uint8_t marker, uint8_t *buffer, size_t len) {
    if (block_num > (WL_BLOCK_NUM - 1)) {
        return 1;
    }

    uint8_t intermediate_buffer[len + 1];

    intermediate_buffer[0] = marker;
    memcpy(&intermediate_buffer[1], buffer, len);
    EE24LC16_SEQUENTIAL_WRITE(eeprom_driver, WL_MARKER_ADDRESS(block_num), intermediate_buffer, len + 1);
    return 0;
}
#endif


static int controller_start_check(void) {
    uint8_t buff = 0;
    size_t  i;
    for (i = 0; i < 5; i++) {
        EE24CL16_SEQUENTIAL_READ(eeprom_driver, CHECK_BYTE_ADDRESS, &buff, 1);
        if (buff == CHECK_BYTE) {
            return 1;
        }
    }
    return 0;
}