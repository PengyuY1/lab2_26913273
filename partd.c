/*
 * GccApplication6.c
 *
 * Created: 2021/9/30 17:32:04
 * Author : 大能耐
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

volatile int start = 0;
volatile int end = 0;
volatile int period = 0;
volatile int state = 0;
volatile int i = 0;
char string1[25];

void Initialize(){
	cli();					//Disable global interrupts
	DDRB &= ~(1<<DDB0);		//pin8, pb0 input
	
	//Timer 1 setup /1024
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	
	//set timer to normal
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1A &= ~(1<<WGM12);
	TCCR1A &= ~(1<<WGM13);
	
	TCCR1B &= ~(1<<ICES1);	//looking for falling edge
	
	TIFR1 |= (1<<ICF1);		//clear interrupt flag
	
	TIMSK1 |= (1<<ICIE1);	//enable input capture interrupt
	
	sei();					//enable all global interrupt
	
	
}

void FindResult(){
	//480,000/1024 = 468.75 = 469
	//3,200,000/1024 = 3125
	//6,400,000/1024 = 6250
	if(period >= 469 && period < 3125){
		state = 0;
	}else if(period >= 3125 && period < 6250){
		state = 1;
	}else if(period >= 6250){
		state = 2;
	}
}

void printR(){
	if(state == 0){
		sprintf(string1,"dot %u\n", period);
	}else if(state == 1){
		sprintf(string1,"dash %u\n", period);
	}else{
		sprintf(string1,"space %u\n", period);
	}
	UART_putstring(string1);
}

ISR(TIMER1_CAPT_vect){
	i = 0;
	period = TCNT1;
	if(!PINB & (1<<DDB0)){	//press button
		start = TCNT1;
		i=1;
	}else{					//release button
		end = TCNT1;
		i=2;				//give result after we get end
	}
	
	period = end - start;
	
	TCCR1B ^= (1<<ICES1);	//capture the opposite edge
	
	if(i == 2){
		TCNT1 = 0;				//reset
		FindResult();			//calculate result
		printR();				//print result
	}
}


int main(void)
{
	UART_init(BAUD_PRESCALER);
	Initialize();
	while (1)
	{
	}
}
