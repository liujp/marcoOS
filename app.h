#ifndef _APP_H
#define _APP_H

#include "CORE.H"

/* values*/
typedef struct {
	uint8_t led_key;
    uint8_t events;
	uint16_t voltage;
} value_t;

/*define 4 tasks*/
#define TaskList(_) _(CHARGE) _(DISPLAY) _(KEY_SCAN) _(PWM)
/*define three flags max 8*/
#define EventList(_) _(LOW_VOLTAGE) = 0,  _(KEY_SCAN), _(PWM), _(SOS)
TaskListInit(TaskList);
EventListInit(EventList);

#endif // end header
