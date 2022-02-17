#include "app.h"
#include "hardware.H"


TDef(CHARGE) {
	uint16_t sum = 0;
	for(uint8_t k = 0; k < 8; k++){
		sum += GetVoltage();
	}

	if(IS_PLUGIN())/*in charge and the light can not be on*/ {
		if((sum >> 3) > ((value_t*)para)->voltage)
			((value_t *)para)->voltage = sum >> 3;
	} else/*dis charge*/ {
		if((sum >> 3) < ((value_t*)para)->voltage || ((value_t*)para)->voltage == 0) 
			((value_t *)para)->voltage = sum >> 3;
	}

	if(((value_t*)para)->voltage < 682)/*LT 3000mv*/
		SET_BITMAP_BIT(((value_t*)para)->events, EVENT_LOW_VOLTAGE);
	else
	    CLEAR_BITMAP_BIT(((value_t*)para)->events, EVENT_LOW_VOLTAGE);
}

static uint8_t ElectricityStatus(value_t *values) {	 
	/*delta = (4250 - 3000) / 4 = 312.5V*/
	if(IS_PLUGIN())/*if plugin in charge*/{		
    	if((682 <= values->voltage) && (values->voltage < 753))/*3000--3312 1V=170.7*/{
			return 1;
		} else if((753 <= values->voltage) && (values->voltage < 824))/*3312--3624*/{
			return 2;
		} else if((values->voltage >= 824)&&(values->voltage < 895))/*3624--3936*/{
			return 3;
		}else if((values->voltage >= 895)&&(values->voltage <= 955))/*3936--4200*/{
			return 4;
		} else if(values->voltage > 955)/*> 4200*/ {
			return 5; // return 5 is full on 
		}
	} 
	/*not plugin*/
	/*low volatge*/
	if(IS_BITMAP_SET_BIT(values->events, EVENT_LOW_VOLTAGE)) 
		return 0; // return 0 is full off
	/*hight voltage and light on*/
	if(values->led_key > 0) {
		if((682 <= values->voltage) && (values->voltage < 753))/*3.0--3.50*/{
			return 6;
		} else if((753 <= values->voltage) && (values->voltage < 824))/*3.50-3.70*/{
			return 7;
		} else if((values->voltage >= 824) && (values->voltage <= 895))/*3.70--3.90*/{
			return 8;
		} else if (values->voltage > 895) {
			return 5;
		}
	}
	return 0;
}

TDef(DISPLAY) {
	uint8_t T_cont = 0 ;
	T_cont = ElectricityStatus((value_t*)para);
	switch(T_cont) {
		case 0: /*all off*/SET(LED1); SET(LED2); SET(LED3); SET(LED4); break;
		case 1: /*flash 1*/LED1 = !LED1; SET(LED2); SET(LED3); SET(LED4); break;
		case 2: /*flash 2*/CLEAR(LED1); LED2 = !LED2; SET(LED3); SET(LED4); break;
		case 3: /*flash 3*/CLEAR(LED1); CLEAR(LED2); LED3 = !LED3; SET(LED4); break;
		case 4: /*flash 4*/CLEAR(LED1); CLEAR(LED2); CLEAR(LED3); LED4 = !LED4; break;
		case 5: /*all on */CLEAR(LED1); CLEAR(LED2); CLEAR(LED3); CLEAR(LED4); break;
		case 6: /*1 one  */CLEAR(LED1); SET(LED2); SET(LED3); SET(LED4); break;
		case 7: /*1/2 on */CLEAR(LED1); CLEAR(LED2); SET(LED3); SET(LED4); break;
		case 8: /*1/2/3on*/CLEAR(LED1); CLEAR(LED2); CLEAR(LED3); SET(LED4); break;
		default: break;
	}
}

static void AdjustLED(value_t *para) {
	switch(para->led_key) {
		case 0: 
			PR3L = 0;
			if(!IS_PLUGIN()) {
				SET(LED1);
				SET(LED2);
				SET(LED3);
				SET(LED4);
				_SLEEP_;
				_NOP_;
				_NOP_;
			}
			break;
		default:
		    if(IS_BITMAP_CLEAR_BIT(para->events, EVENT_PWM))
		        SET_BITMAP_BIT(para->events, EVENT_PWM);
			break;
	}
}

TDef(KEY_SCAN) {
	if(IS_BITMAP_SET_BIT(((value_t*)para)->events, EVENT_KEY_SCAN)) {
		if(IS_BITMAP_SET_BIT(((value_t*)para)->events, EVENT_LOW_VOLTAGE)) {
			((value_t*)para)->led_key = 0;
		} else {
			if(!IS_PLUGIN()) {
				((value_t*)para)->led_key ++;
				if(((value_t*)para)->led_key > 4)
					((value_t*)para)->led_key = 0;
			} else { /*impossible branch*/
				((value_t*)para)->led_key = 0;
			}
		}
		AdjustLED((value_t*)para);
		CLEAR_BITMAP_BIT(((value_t*)para)->events, EVENT_KEY_SCAN);
	}
}

TDef(PWM) {
	value_t* p = (value_t*)para;
	if(IS_BITMAP_SET_BIT(p->events, EVENT_PWM)) {
		// CLEAR_BITMAP_BIT(p->events, EVENT_PWM);
		switch(p->led_key) {
			case 1:
				PR3L = 200;
				break;
			case 2:
				PR3L = 100;
				break;
			case 3:
			    PR3L = 50;
				break;
			case 4:
				PR3L = IS_BITMAP_SET_BIT(p->events, EVENT_SOS)?
				(CLEAR_BITMAP_BIT(p->events, EVENT_SOS), 0):
				(SET_BITMAP_BIT(p->events, EVENT_SOS), 255);

		}
	}
}
