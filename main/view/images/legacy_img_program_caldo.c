#include "legacy.h"

static uint8_t const bitmap[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,  /*  ................................................  */
    0x00,0x01,0xFF,0xFF,0x80,0x00,  /*  ...............%%%%%%%%%%%%%%%%%%...............  */
    0x01,0xFE,0x00,0x00,0x7F,0x80,  /*  .......%%%%%%%%..................%%%%%%%%.......  */
    0x0E,0x00,0x00,0x00,0x00,0x70,  /*  ....%%%..................................%%%....  */
    0x10,0x00,0x0F,0xF0,0x00,0x08,  /*  ...%................%%%%%%%%................%...  */
    0x10,0x00,0x10,0x08,0x00,0x08,  /*  ...%...............%........%...............%...  */
    0x10,0x00,0x10,0x08,0x00,0x08,  /*  ...%...............%........%...............%...  */
    0x20,0x00,0x11,0x88,0x00,0x04,  /*  ..%................%...%%...%................%..  */
    0x20,0x00,0x1B,0xC8,0x00,0x04,  /*  ..%................%%.%%%%..%................%..  */
    0x20,0x00,0x13,0xC8,0x00,0x04,  /*  ..%................%..%%%%..%................%..  */
    0x20,0x00,0x13,0xC8,0x00,0x04,  /*  ..%................%..%%%%..%................%..  */
    0x20,0x00,0x13,0xC8,0x00,0x04,  /*  ..%................%..%%%%..%................%..  */
    0x20,0x00,0x13,0xC8,0x00,0x04,  /*  ..%................%..%%%%..%................%..  */
    0x20,0x00,0x1B,0xC8,0x00,0x04,  /*  ..%................%%.%%%%..%................%..  */
    0x20,0x00,0x13,0xC8,0x00,0x04,  /*  ..%................%..%%%%..%................%..  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x1B,0xC8,0x00,0x02,  /*  .%.................%%.%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x1B,0xC8,0x00,0x02,  /*  .%.................%%.%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x1B,0xC8,0x00,0x02,  /*  .%.................%%.%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x40,0x00,0x13,0xC8,0x00,0x02,  /*  .%.................%..%%%%..%.................%.  */
    0x20,0x00,0x13,0xC8,0x00,0x04,  /*  ..%................%..%%%%..%................%..  */
    0x20,0x00,0x23,0xC4,0x00,0x04,  /*  ..%...............%...%%%%...%...............%..  */
    0x20,0x00,0x47,0xE2,0x00,0x04,  /*  ..%..............%...%%%%%%...%..............%..  */
    0x20,0x00,0x4F,0xF2,0x00,0x04,  /*  ..%..............%..%%%%%%%%..%..............%..  */
    0x20,0x00,0x4F,0xF2,0x00,0x04,  /*  ..%..............%..%%%%%%%%..%..............%..  */
    0x20,0x00,0x4F,0xF2,0x00,0x04,  /*  ..%..............%..%%%%%%%%..%..............%..  */
    0x20,0x00,0x4F,0xF2,0x00,0x04,  /*  ..%..............%..%%%%%%%%..%..............%..  */
    0x20,0x00,0x47,0xE2,0x00,0x04,  /*  ..%..............%...%%%%%%...%..............%..  */
    0x10,0x00,0x23,0xC4,0x00,0x08,  /*  ...%..............%...%%%%...%..............%...  */
    0x10,0x00,0x10,0x08,0x00,0x08,  /*  ...%...............%........%...............%...  */
    0x10,0x00,0x0F,0xF0,0x00,0x08,  /*  ...%................%%%%%%%%................%...  */
    0x0E,0x00,0x00,0x00,0x00,0x70,  /*  ....%%%..................................%%%....  */
    0x01,0xFE,0x00,0x00,0x7F,0x80,  /*  .......%%%%%%%%..................%%%%%%%%.......  */
    0x00,0x01,0xFF,0xFF,0x80,0x00,  /*  ...............%%%%%%%%%%%%%%%%%%...............  */
    0x00,0x00,0x00,0x00,0x00,0x00   /*  ................................................  */
};


const GSYMBOL legacy_img_program_caldo = {
    .sh   = {1, 48, 48},
    .data = (uint8_t *)bitmap,
};