#ifndef VIEW_TYPES_H_INCLUDED
#define VIEW_TYPES_H_INCLUDED

#include <stdint.h>
#include "gel/keypad/keypad.h"


#define VIEW_EMPTY_MSG                                                                                                 \
    ((view_message_t){.vmsg = {.code = VIEW_PAGE_COMMAND_CODE_NOTHING},                                                \
                      .cmsg = {.code = VIEW_CONTROLLER_COMMAND_CODE_NOTHING}})


typedef enum {
    VIEW_PAGE_COMMAND_CODE_NOTHING = 0,
    VIEW_PAGE_COMMAND_CODE_REBASE,
    VIEW_PAGE_COMMAND_CODE_SWAP_PAGE,
    VIEW_PAGE_COMMAND_CODE_SWAP_PAGE_EXTRA,
    VIEW_PAGE_COMMAND_CODE_CHANGE_PAGE,
    VIEW_PAGE_COMMAND_CODE_CHANGE_PAGE_EXTRA,
    VIEW_PAGE_COMMAND_CODE_BACK,
    VIEW_PAGE_COMMAND_CODE_BACK_TWICE,
    VIEW_PAGE_COMMAND_CODE_UPDATE,
} view_page_command_code_t;

typedef struct {
    view_page_command_code_t code;

    union {
        struct {
            const void *page;
            void       *extra;
        };
    };
} view_page_command_t;


typedef enum {
    VIEW_CONTROLLER_COMMAND_CODE_NOTHING = 0,
} view_controller_command_code_t;


typedef struct {
    view_controller_command_code_t code;
    union {
        struct {
            int output;
            int value;
        };
    };
} view_controller_command_t;


typedef struct {
    view_page_command_t       vmsg;
    view_controller_command_t cmsg;
} view_message_t;

typedef uint8_t view_t;


typedef enum {
    VIEW_EVENT_KEYPAD,
    VIEW_EVENT_MODEL_UPDATE,
    VIEW_EVENT_CODE_TIMER,
} view_event_code_t;


typedef struct {
    view_event_code_t code;
    union {
        keypad_update_t key_event;
        int             timer_id;
        unsigned int    coins;
    };
} view_event_t;

#endif
