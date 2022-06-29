#ifndef LEGACY_H_INCLUDED
#define LEGACY_H_INCLUDED

#include <stdlib.h>

#include "lvgl/lvgl.h"


#define LEGACY_CANVAS_BUFFER_SIZE(x) LV_CANVAS_BUF_SIZE_INDEXED_1BIT(x.sh.cxpix, x.sh.cypix)


typedef struct _GSYMHEAD /* Symbol header */
{
    uint8_t reverse; /* Symbol ID (not used by GLCD)*/
    size_t  cxpix;   /* Symbol size in no X pixels */
    size_t  cypix;   /* Symbol size in no Y pixels */
} GSYMHEAD, *PGSYMHEAD;

typedef struct _GSYMBOL /* One table entry */
{
    GSYMHEAD sh;   /* Symbol header */
    uint8_t *data; /* Symbol data, variable length = (cxpix/8+1)*cypix */
} GSYMBOL, *PGSYMBOL;



#endif  /* LEGACY_H_INCLUDED */
