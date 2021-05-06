/*	Author: Maverick Bautista
 *  	Partner(s) Name: 
 *	Lab Section: Section 023
 *	Assignment: Lab 8  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/WwRUhkTdgww 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn () {
	
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States { Wait, Play, Button } State;
unsigned char i = 0x00; //index
unsigned char tick = 0x00; //tick
unsigned char but = 0x00; //button
const double notes[] = { 440.00, 293.66, 329.63, 349.23, 392.00, 329.63, 349.23, 293.66, 349.23, 392.00, 440.00, 493.88, 392.00, 440.00, 349.23 }; //notes, total of 15
const double time[] = {2, 1, 1, 4, 2, 4, 2, 10, 1, 1, 4, 2, 4, 2, 10 }; //time per note, 15 total
void TickFct() {
	switch(State) {
		case Wait:
			if(but) {
				State = Play;
				i = 0; //i = 0 to start off on the first note
			}
			else {
				State = Wait;
			}
		break;

		case Play:
			if(i < 15) { //set to 15
				if(tick < time[i]) { 
					State = Play;
				}
		
				else {
					tick = 0x00;
					++i;
					State = Play;
				}
			}
			else {
				State = Button;
			}
		break;

		case Button: //for button presses during/after melody
			if(but) {
				State = Button;
			}
			else {
				State = Wait;
			}
		break;

		default:
			State = Wait;
		break;

	}

	switch(State) {
		case Play:
			set_PWM(notes[i]); //plays that frequency
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
	TimerSet(100); //if set to 100, plays for 5 secs, while 150 will make it play for 7.5s which matches more in line with the original song.
	TimerOn();	
	PWM_on();
	State = Wait;

    while (1) {
	but = ~PINA & 0x01; //stands for button
	TickFct();
	++tick;
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
