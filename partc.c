/*
 * GccApplication4.c
 *
 * Created: 2021/9/24 15:13:28
 * Author : 大能耐
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

void Initialize(){
	cli();					//disable all global interrupt
	DDRB |= (1<<DDB5);		//pin 13, PB5 output
	DDRB &= ~(1<<DDB0);		//pin 8, PB0 input
	PORTB &= ~(1<<PORTB5);	//PB5 initial 0

	//Set Timer to normal
	TCCR1A  &= ~(1<<WGM10);
	TCCR1A  &= ~(1<<WGM11);
	TCCR1A  &= ~(1<<WGM12);
	TCCR1A  &= ~(1<<WGM13);
	
	TCCR1B &= ~(1<<ICES1);	//looking for falling edge
	
	TIFR1 |= (1<<ICF1);		//clear interrupt flag
	
	TIMSK1 |= (1<<ICIE1);	//enable input capture interrupt 
	
	sei();					//enable all global interrupt
}

ISR(TIMER1_CAPT_vect){
	PORTB ^= (1<<PORTB5); // light LED to show interrupt
	TCCR1B ^= (1<<ICES1);
}

int main(void)
{
    Initialize();
    while (1) 
    {
    }
}

