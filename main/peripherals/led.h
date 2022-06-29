#ifndef LED_H_INCLUDED
#define LED_H_INCLUDED



typedef enum
{
    LED_COLOR_BLACK     = 0x00,
    
    LED_COLOR_RED       = 0x02,
    LED_COLOR_GREEN     = 0x01,
    LED_COLOR_BLUE      = 0x04,
    
    LED_COLOR_YELLOW    = 0x03,
    LED_COLOR_PURPLE    = 0x06,
    LED_COLOR_TURQUOISE = 0x05,
    
    LED_COLOR_WHITE     = 0x07,
} led_color_t;



void led_init(void);
void led_set_color(led_color_t color);

#endif
