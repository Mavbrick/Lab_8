/*	Author: Maverick Bautista
 *  	Partner(s) Name: 
 *	Lab Section: Section 023
 *	Assignment: Lab 8  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/AwuK_D69Qn8
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }

		else if (frequency > 31250) { OCR3A = 0x0000; }

		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States { Initial, Silent, C, D, E } State;
unsigned char but = 0x00;

void TickFct() {
	switch(State) {
		case Initial:
			State = Silent;	
		break;

		case Silent:
			if(but == 0x01) {
				State = C;
			}
			else if(but == 0x02) {
				State = D;
			}
			else if(but == 0x04) {
				State = E;
			}
			else{
				State = Silent;
			}
		break;

		case C:
			if(but == 0x01) {
				State = C;
			}
			else {
				State = Silent;
			}
		break;

		case D:
			if(but == 0x02) {
				State = D;
			}
			else {
				State = Silent;
			}
		break;

		case E:
			if(but == 0x04) {
				State = E;
			}
			else {
				State = Silent;
			}
		break;

		default:
			State = Silent;
		break;
	}

	switch(State) {
		case Initial:
		break;

		case Silent:
			set_PWM(0);
		break;

		case C:
			set_PWM(261.63);
		break;

		case D:
			set_PWM(293.66);
		break;

		case E:
			set_PWM(329.63);
		break;

		default:
		break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x40;
    /* Insert your solution below */
	PWM_on();
	State = Initial;
    while (1) {
	but = ~PINA & 0x07; //stands for button
	TickFct();
    }
    return 1;
}
