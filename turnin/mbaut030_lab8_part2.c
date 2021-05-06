/*	Author: Maverick Bautista
 *  	Partner(s) Name: 
 *	Lab Section: Section 023
 *	Assignment: Lab 8  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/z-L3p1y_qBA
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

enum States { Initial, Wait, Off, Power, Inc, Release1, Dec, Release2, On} State;
unsigned char i = 8; //value of 8
unsigned char but = 0x00;
const double notes[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25 };

void TickFct() {
	switch(State) {
		case Initial:
			State = Off;	
		break;

		case Wait:
			if(but == 0x01) {
				State = Inc;
			}
			else if(but == 0x02) {
				State = Dec;
			}
			else if(but == 0x04) {
				State = Power;
			}
			else {
				State = Wait;
			}
		break;

		case Off:
			if(but == 0x04) {
				State = On;
			}
			else {
				State = Off;
			}	
		break;

		case Power:
			if(but == 0x04) {
				State = Power;
			}
			else {
				State = Off;
			}
		break;

		case On:
			if(but == 0x04) {
				State = On;
			}
			else {
				State = Wait;
			}
		break;

		case Inc:
			State = Release1;
		break;

		case Release1:
			if(but == 0x01) {
				State = Release1;
			}
			else {
				State = Wait;
			}
		break;

		case Dec:
			State = Release2;
		break;

		case Release2:
			if(but == 0x02) {
				State = Release2;
			}
			else {
				State = Wait;
			}
		break;

		default:
			State = Initial;
		break;

	}
	switch(State) {
		case Off:
			PWM_off();
		break;

		case On:
			PWM_on();
			i = 0x00;
		break;

		case Inc:
			if(i < 7) {
				++i;
			}
		break;

		case Dec:
			if(i > 0) {
				--i;
			}	
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
	set_PWM(notes[i]);
    }
    return 1;
}
