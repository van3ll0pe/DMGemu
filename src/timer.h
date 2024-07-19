#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include "bus.h"

void handle_timer(Bus* bus, uint8_t elapsed_Tcycles);

#endif