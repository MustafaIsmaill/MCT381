#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

void init(void);
void stepper(int);
void gripper_toggle(void);

const int delay = 20;
volatile char step[4] = {0x50,0x60,0xA0,0x90};
volatile int count=0;
volatile int INT0count=0;
volatile int INT1count=0;

int main(void){  
	init();
	_delay_ms(5000);

	sei(); 
	//OCR1B = 316;
	while(1){
	}
}

ISR(INT1_vect){ //RIGHT
	INT1count++;
	if(INT1count==10000){
		_delay_ms(100);
		stepper(50);
		_delay_ms(100);
		gripper_toggle();
		_delay_ms(100);
		stepper(-50);
		_delay_ms(100);
		gripper_toggle();
		_delay_ms(5000);
		INT1count=0;
	}
}
ISR(INT0_vect){ //LEFT
	INT0count++;
	if(INT0count==10000){	
		_delay_ms(100);
		stepper(-50);
		_delay_ms(100);
		gripper_toggle();
		_delay_ms(100);
		stepper(50);
		_delay_ms(100);
		gripper_toggle();
		_delay_ms(5000);
		INT0count=0;
	}
}

void init(void){
	DDRD = 0xF0; //pins 4,5,6,7 ... Arduino pins 4,5,6,7
	PORTD |= (1<<2)|(1<<3); //enable pull up
	EIMSK |= 0x03; //interrupt enable

	DDRB = 0x04; //pin 3 ... Arduino pin 10
	DDRB |=(1<<4);

	//Configure TIMER1
	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)

	ICR1=4999; //PWM=50Hz (Period = 20ms Standard).
	OCR1B = 520;
}

void stepper(int steps){
	if(steps>0){
		for(int k=0;k<steps;k++){
			PORTD = step[count];
			_delay_ms(delay);
			count++;
			if(count == 4){ count = 0;}
		}
	}
	else if(steps<0){
		for(int k=0;k>steps;k--){
			PORTD = step[count];
			_delay_ms(delay);
			count--;
			if(count == -1){ count = 3;}
		}
	}
}

void gripper_toggle(void){
	switch (OCR1B){
		case 316:
			for(int i=0;i<204;i++){	
				OCR1B++;
				_delay_ms(5);
			}
			break;
		case 520:
			for(int i=0;i<204;i++){
				OCR1B--;
				_delay_ms(5);
			}
			break;
	}
}


/*
PORTD = (1<<3)|(1<<5);
_delay_ms(delay);
PORTD = (1<<4)|(1<<5);
_delay_ms(delay);
PORTD = (1<<4)|(1<<6);
_delay_ms(delay);
PORTD = (1<<3)|(1<<6);
_delay_ms(delay);
*/
