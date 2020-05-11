/*	Author: Garrett Yamano
 *  Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Lab #7 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "timer.h"

enum States {Start, Blink, Button1, Release1, Button2, Result} state;

unsigned char cnt;
unsigned char down;
unsigned char score;

void Tick(){
	switch(state){ // Transitions
		case Start: // Initial transition
			down = 0x00;
			score = 0x05;
			cnt = 0x00;
			PORTB = 0x01;
			LCD_WriteData(score + '0'); // will display score on the LCD
			state = Blink;
			break;
		case Blink:
			if((~PINA & 0xFF) == 0x00){
				if(cnt == 0x01){
					PORTB = 0x02;
				}
				else if(cnt == 0x02){
					PORTB = 0x04;
				}
				else{
					PORTB = 0x01;
				}
				state = Blink;
			}
			else{
				state = Button1;
			}
			break;
		case Button1:
			if((~PINA & 0xFF) == 0x01){
				state = Button1;
			}
			else{	
				if((PORTB & 0xFF) == 0x02){
                                	if(score < 9){
                                        	score++;
                                	}
					LCD_Cursor(1);
                                	LCD_WriteData(score + '0'); // will display score on the LCD
                                	if(score == 0x09){
                                        	LCD_DisplayString(1, "Winner Winner Chicken Dinner");
						if((PINA & 0xFF) == 0x01){
							LCD_ClearScreen();
							state = Blink;
						}
                                	}
                        	}
                        	else if(((PORTB & 0xFF) == 0x01) || ((PORTB & 0xFF) == 0x04)){
                                	if(score > 0){
                                        	score--;
                                	}
					LCD_Cursor(1);
                                	LCD_WriteData(score + '0'); // will display score on the LCD
                        	}
				state = Release1;
			}
			break;
		case Release1:
			if((~PINA & 0xFF) == 0x00){
				state = Release1;
			}
			else{
				state = Button2;
			}
			break;
		case Button2:
			if((~PINA & 0xFF) == 0x01){
				state = Button2;
			}
			else{
				PORTB = 0x01;
				down = 0x01;
				cnt = 0x01;
				state = Blink;
			}
			break;
		/*case Result:
			if((PORTB & 0xFF) == 0x02){
				if(score < 9){
					score++;
				}
				LCD_WriteData(score + '0'); // will display score on the LCD
				if(score == 0x09){
					LCD_DisplayString(1, "Winner Winner Chicken Dinner");
				}
			}
			else if(((PORTB & 0xFF) == 0x01) || ((PORTB & 0xFF) == 0x04)){
				if(score > 0){
					score--;
				}
				LCD_WriteData(score + '0'); // will display score on the LCD
			}
			break;*/
		default:
			state = Start;
			break;
	} // Transitions
	switch(state){ // State actions
		case Blink:
			if(down){
				cnt--;
				if(cnt == 0x00){
					down = 0x00;
				}
			}
			else{
				cnt++;
				if(cnt == 0x02){
					down = 0x01;
				}
			}
			break;
		case Button1:
			break;
		case Release1:
			break;
		case Button2:
			break;
		default:
			break;
	} // State actions
}

int main(void) {
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	DDRA = 0x00; // Set port A to input
	PORTA = 0xFF; // Init port A to 1s
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRD = 0xFF;
	PORTD = 0x00;
	LCD_init();
	state = Start;
	TimerSet(300);
	TimerOn();
	while (1) {
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}
