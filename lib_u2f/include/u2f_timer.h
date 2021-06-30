
/* @BANNER@ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef __U2F_TIMER_H__

#define __U2F_TIMER_H__

typedef void (*u2fTimer_t)(struct u2f_service_t *service);

void u2f_timer_init(void);
void u2f_timer_register(uint32_t timerMs, u2fTimer_t timerCallback);
void u2f_timer_cancel(void);

#endif
