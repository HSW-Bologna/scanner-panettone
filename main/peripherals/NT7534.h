#ifndef NT7534_H_INCLUDED
#define NT7534_H_INCLUDED

#include "lvgl/lvgl.h"
#include <stdint.h>



#define NT7534_DEFAULT_CONTRAST 26



void nt7534_init(void);
void nt7534_set_px(struct _disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                   lv_color_t color, lv_opa_t opa);
void nt7534_rounder(struct _disp_drv_t *disp_drv, lv_area_t *a);
void nt7534_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void nt7534_sync(uint8_t *data, lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2);
void nt7534_reconfigure(uint8_t contrast);

#endif  /* NT7534_H_INCLUDED */
