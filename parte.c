/*
 * GccApplication5.c
 *
 * Created: 2021/9/29 19:43:13
 * Author : 大能耐
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"

volatile int start = 0;
volatile int end = 0;
volatile int period = 0;
volatile int state = 0;
volatile int i = 0;
char string1[25];
char morse[8];
char letter;

void Initialize(){
	cli();					//Disable global interrupts
	DDRB &= ~(1<<DDB0);		//pin8, pb0 input
	DDRB |= (1<<DDB5);		//pin13, pb5, red led, dot
	DDRB |= (1<<DDB4);		//pin12, pb4, yellow led, dash
	
	PORTB &= ~(1<<PORTB5);	//initial 0
	PORTB &= ~(1<<PORTB4);	
	
	//Timer 1 setup /1024
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	
	//set timer to normal
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	
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

char decode(char String[]){
	char a; // return letter
	if(strcmp(String, ".-") == 0){
		a = 'A';
	}else if(strcmp(String,"-...") == 0){
		a = 'B';
	}else if(strcmp(String,"-.-.") == 0){
		a = 'C';
	}else if(strcmp(String,"-..") == 0){
		a = 'D';
	}else if(strcmp(String,".") == 0){
		a = 'E';
	}else if(strcmp(String,"..-.") == 0){
		a = 'F';
	}else if(strcmp(String,"--.") == 0){
		a = 'G';
	}else if(strcmp(String,"....") == 0){
		a = 'H';
	}else if(strcmp(String,"..") == 0){
		a = 'I';
	}else if(strcmp(String,".---") == 0){
		a = 'J';
	}else if(strcmp(String,"-.-") == 0){
		a = 'K';
	}else if(strcmp(String,".-..") == 0){
		a = 'L';
	}else if(strcmp(String,"--") == 0){
		a = 'M';
	}else if(strcmp(String,"-.") == 0){
		a = 'N';
	}else if(strcmp(String,"---") == 0){
		a = 'O';
	}else if(strcmp(String,".--.") == 0){
		a = 'P';
	}else if(strcmp(String,"--.-") == 0){
		a = 'Q';
	}else if(strcmp(String,".-.") == 0){
		a = 'R';
	}else if(strcmp(String,"...") == 0){
		a = 'S';
	}else if(strcmp(String,"-") == 0){
		a = 'T';
	}else if(strcmp(String,"..-") == 0){
		a = 'U';
	}else if(strcmp(String,"...-") == 0){
		a = 'V';
	}else if(strcmp(String,".--") == 0){
		a = 'W';
	}else if(strcmp(String,"-..-") == 0){
		a = 'X';
	}else if(strcmp(String,"-.--") == 0){
		a = 'Y';
	}else if(strcmp(String,"--..") == 0){
		a = 'Z';
	}else if(strcmp(String,".----") == 0){
		a = '1';
	}else if(strcmp(String,"..---") == 0){
		a = '2';
	}else if(strcmp(String,"...--") == 0){
		a = '3';
	}else if(strcmp(String,"....-") == 0){
		a = '4';
	}else if(strcmp(String,".....") == 0){
		a = '5';
	}else if(strcmp(String,"-....") == 0){
		a = '6';
	}else if(strcmp(String,"--...") == 0){
		a = '7';
	}else if(strcmp(String,"---..") == 0){
		a = '8';
	}else if(strcmp(String,"----.") == 0){
		a = '9';
	}else if(strcmp(String,"-----") == 0){
		a = '0';
	}else{
		a = '*';
	}
	return a;
	
}

void printR(){
	if(state == 0){
		//sprintf(string1,"dot %u\n", period);
		//turn on and of led
		PORTB |= (1<<PORTB5);
		_delay_ms(30);
		PORTB &= ~(1<<PORTB5);
		
		//morse[0] = '.';
		strcat(morse,".");
 	}else if(state == 1){
		//sprintf(string1,"dash %u\n", period);
		//turn on and of led
		PORTB |= (1<<PORTB4);
		_delay_ms(30);
		PORTB &= ~(1<<PORTB4);
		
		//morse[0] = '-';
		strcat(morse,"-");
	}else{
		//sprintf(string1,"space %u\n", period);
		letter = decode(morse);
		sprintf(string1,"%c\n", letter);
		UART_putstring(string1);
		memset(morse,'\0',sizeof(morse));	//reset morse
	}
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