/*	Author: gyama009
 *  Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Lab #7 Exercise #1
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

enum States{Start, Init, Increment, Decrement, Reset} state;

unsigned char holdval;
unsigned char cnt;

void Tick(){
	switch(state){
		case Start: // Initial transition
			holdval = 0x00;
			cnt = 0x00;
			LCD_Cursor(1);
			LCD_WriteData(holdval + '0'); // will display 0 on the LCD
			state = Init;
			break;
		case Init:
			if((~PINA & 0x03) == 0x03){
				state = Reset;
				holdval = 0x00;
				LCD_Cursor(1);
				LCD_WriteData(holdval + '0'); // will display 0 on the LCD
			}
			else if(((~PINA & 0x02) == 0x02) && (holdval > 0)){
				state = Decrement;
			}
			else if(((~PINA & 0x01) == 0x01) && (holdval < 9)){
				state = Increment;
			}
			else{
				state = Init;
			}
			break;
		case Increment:
			if((~PINA & 0xFF) == 0x00){
				state = Init;
			}
			else if((~PINA & 0x03) == 0x03){
				state = Reset;
				cnt = 0x00;
				holdval = 0x00;
				LCD_Cursor(1);
				LCD_WriteData(holdval + '0'); // will display 0 on the LCD
			}
			else{
				state = Increment;
			}
			break;
		case Decrement:
			if((~PINA & 0xFF) == 0x00){
				state = Init;
			}
			else if((~PINA & 0x03) == 0x03){
				state = Reset;
				holdval = 0x00;
				LCD_Cursor(1);
				LCD_WriteData(holdval + '0'); // will display 0 on the LCD
			}
			else{
				state = Decrement;
			}
			break;
		case Reset:
			if(((~PINA & 0x03) == 0x03)){
				state = Reset;
				//PORTB = 0x00;
			}
			else if(((~PINA & 0x01) == 0x01)){
				state = Increment;
				//PORTB += 1;
			}
			else if(((~PINA & 0x02) == 0x02)){
				state = Decrement;
				//PORTB -= 1;
			}
			else{
				state = Init;
			}
			break;
		default:
			state = Start;
			break;
	} // Transitions
	switch(state){ // State actions
		case Init:
			break;
		case Increment:
			cnt++;
			if(cnt == 0x0A){
				if(holdval < 9){
					holdval++;
					LCD_Cursor(1);
					LCD_WriteData(holdval + '0'); // will display 0 on the LCD
				}
				cnt = 0x00;
			}
			break;
		case Decrement:
			if(holdval > 0){
				holdval--;
				LCD_Cursor(1);
				LCD_WriteData(holdval + '0'); // will display 0 on the LCD
			}
			break;
		case Reset:
			holdval = 0x00;
			LCD_Cursor(1);
			LCD_WriteData(holdval + '0'); // will display 0 on the LCD
			break;
		default:
			break;
	} // State actions
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(0);
	state = Start;
	TimerSet(100);
	TimerOn();
	while (1) {
		Tick();	
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 0;
}
