/*
 * GccApplication3.c
 *
 * Created: 2021/9/24 14:18:22
 * Author : 大能耐
 */ 

#include <avr/io.h>

void Initialize(){
	DDRB &= ~(1<<DDB0); // input pin 8 (PB0)
	DDRB |= (1<<DDB5); // pin 13 for led (PB5)
}

int main(void)
{
    Initialize();
    while (1) 
    {
		if(PINB & (1<<PINB0)){
			PORTB &= ~(1<<PORTB5); //high
		}else{
			PORTB |= (1<<PORTB5); //low
		}
	}
}

