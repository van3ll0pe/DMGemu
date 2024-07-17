#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

typedef enum
{
    JOYPAD = 0x10,
    SERIAL = 0x08,
    TIMER = 0x04,
    LCD = 0x02,
    VBLANK = 0x01
}interrupts;



#endif //__INTERRUPTS_H__