 #include "hardware.H"

void InitOsc() {
    OSCCON |= 0xe1;//2T 8M: LFMOD: 256K, FOSC = 8M
	PSA = 0;//PSA FOR T0
	WDTCON = 0x16;//off watch dog,32K/65536. 2.048 sec TESET TIME 
}

void InitIo() {
	ANSEL = 0x04;//AN2(PC5)
	CMCON0 |= 0x07;//COM off
	TRISA = 0x40;//PA6 as input, others as output 
	TRISC = 0x22;//PC1 input and others output
	TRISB = 0X00;
	PORTA = 0x00;
	PORTC = 0x00;
	PORTB = 0X00;
	SET(LED1);
	SET(LED2);
	SET(LED3);
	SET(LED4);
}

void InitIsr() {
	INTCON = 0x58;//PERIPHERAL INTERRUPT ENABLE
	IOFA = 0x40;//PA6 interupt enable OTHERS DISABLE
}

void InitTimer2() {
	TMR2 = 0;
	TMR2IE = 1;//T2 enable
	TMR2IF = 0;
	T2CON = 0x7b;//no pre-sca 1:16 POSTSCALE 0111 1011 ,PRESCALE IS 16
	PR2 = 250;//8ms @ 8MHz SYSCLK
	TMR2ON = 1;
}

void InitPwm() {
	EPWMCR1 = 0x01;//PWM3  USE 32M CLK AS SOURCE
	PWM3CR0 = 0x32;//8 BITS PWM 32M/(DIV+1) 0 011 001 0
	PWM3CR1 = 0x80;// disable interrupt
	TMR3H = 0;
	TMR3L = 0;
	PR3L = 0;//INITAL OUTPUT 0
	T3CKDIV = 2;//2+1=3 F=32M/3/256=42kHz
	TMR3ON = 1; // on pwm
}

void InitAdc() {
	ADCON0 = 0b11001000;// right align, inter 3v as ref
	ADCON1 = 0x10;//8M/8 as ADC clock
	ADRESH = 0x00;//clear result registor
	ADRESL = 0x00;//Resolutin: 4.395mv/bit @10bit;
}

void delay(uint8_t cont) {
	for(uint8_t i = 0; i <= cont; i++);
}

uint16_t GetVoltage() {
	uint16_t result;
	ADON = 1;
	delay(30);//delay about 20 us
	LOCK();
	GO_DONE = 1;//begin to convert
	while(GO_DONE);
	ADON = 0;
	UNLOCK();
	result = ADRESH & 0x03;
	result = (result << 8) | ADRESL;
	return result;
}

