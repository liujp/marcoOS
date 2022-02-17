#include "app.h"
#include "hardware.H"

task_t tasks[TASK_NUM];
value_t values;

int main() {
	InitOsc(); 
	InitIo();
	InitPwm();
	InitAdc();
	InitIsr();
	ADDTASK(tasks, CHARGE, 10);
	ADDTASK(tasks, DISPLAY, 80);
	ADDTASK(tasks, KEY_SCAN, 40);
	ADDTASK(tasks, PWM, 50);
	//in order to sleep once on
	InitTimer2();
	UNLOCK();
	_SLEEP_;
	_NOP_;
    while(1) {
		EPOLL(tasks, TASK_NUM, values);
    }
    return 0;
}

void interrupt ISR_Handle() {
	if(TMR2IF)/*T2 interrupt*/ {
		TMR2IF = 0;
		SCHEDULE(tasks, TASK_NUM)
	}
	if(INTF) {
		INTF = 0;
	}	
	if(PAIF){
		PAIF=0;
		if(PA6 == 0) {
			if(!IS_PLUGIN())/*not in charge*/ {
				if(IS_BITMAP_CLEAR_BIT(values.events, EVENT_KEY_SCAN))
				    SET_BITMAP_BIT(values.events, EVENT_KEY_SCAN);
			}
		}
	}	
}