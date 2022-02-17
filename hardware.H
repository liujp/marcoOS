#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "FT61F04X.H"
#include "CORE.H"

// port settings
/*
 *   PIN SETTINGS
 * 
 * PA5 [out] LCD_CLC
 * PA4 [out] LCD_DATA
 * PA2 [out] LCD_RST
 * PC0 [out] LCD_RS
 * PC5 [in]  ADC
 * PC2 [out] PWM3 : reserverd
 * PC1 in charge
 */
#define LED1 PA5
#define LED2 PA4
#define LED3 PA2
#define LED4 PC0
#define LED_KEY PA6

// set/clear pin value
#define SET(PIN) PIN = 1
#define CLEAR(PIN) PIN = 0
#define IS_SET(PIN) (PIN == 1)
#define IS_CLEAR(PIN) (PIN == 0)
#define LOCK() GIE = 0;
#define UNLOCK() GIE = 1;
#define _NOP_ asm("nop")
#define IS_PLUGIN() (PC1 == 1)
#define _SLEEP_ asm("sleep")

/* system event*/
void InitOsc(void);
void InitIo(void);
void InitTimer2(void);
void InitPwm(void);
void InitAdc(void);
void InitIsr(void);
void delay(uint8_t);
uint16_t GetVoltage(); 

#endif