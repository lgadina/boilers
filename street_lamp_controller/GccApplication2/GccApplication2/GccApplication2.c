/*
 * GccApplication2.c
 *
 * Created: 03.10.2015 12:59:19
 *  Author: prosoft
 */ 
#define F_CPU 8000000
#define TIME_DELAY 2 * 10 * 60;
#define ON 1
#define OFF 0
#define DL_ 5

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


unsigned int time_count;
uint8_t lamp_on = OFF;

ISR(TIMER0_COMPA_vect){
	if (time_count > 0) {
		time_count--;	
		if (time_count == 0) {
			lamp_on = OFF;	
		}
	}
	if (lamp_on == ON)	{
		PORTB = 0b11111110;				
	} else
	{				
		PORTB = 0b11111111;
	}
}


//ISR(SIG_INTERRUPT0) {
//	if (bit_is_clear(PINB, PB2))
//	{
//		time_count = 10;		
//	}
//}

int main(void)
{
	PORTB = 0b11111111; // на pin0 0, на остальных включена PullUp
	DDRB = 0b00000001; //pin0 выход, остальные вход
	time_count = 0;
	cli();
	TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00); // делитель 1024
	TCCR0A |= (1 << WGM01); // переполнение
	TIMSK |= (1 << OCIE0A); //	
	OCR0A = 125; // счетчик переполнения
	//TIMSK |= (1 << TOIE0);
//	GIMSK = (1 << INT0) | (0 << PCIE);
//	MCUCR = (1 << ISC01) | (0 << ISC00);
    sei();
    while(1)
    {
        //TODO:: Please write your application code 
		if (bit_is_clear(PINB, PB1)) {
			cli();
			_delay_ms(DL_);
			if (bit_is_clear(PINB, PB1)) {
					lamp_on = ON;
					time_count = TIME_DELAY;
			}
			sei();
		} else
		if (bit_is_set(PINB, PB2)) {
			cli();
			_delay_ms(DL_);
			if (bit_is_set(PINB, PB2)) {
					lamp_on = ON;
					time_count = TIME_DELAY;
			}
			sei();
		} else
		if (bit_is_clear(PINB, PB3)) {
			cli();
			_delay_ms(DL_);
			if (bit_is_clear(PINB, PB3)){
				if (lamp_on == ON) {
					lamp_on = OFF;
					time_count = 0;
				}
				else {
					lamp_on = ON;
				}
				
				do
				{
					
				} while (bit_is_clear(PINB, PB3));
				
			}
		  sei();	
		} else
		if (bit_is_set(PINB, PB4)) {
			cli();
			_delay_ms(DL_);
			if (bit_is_set(PINB, PB4)) {
				if (lamp_on == ON) {
					lamp_on = OFF; 
					time_count = 0;
				}
				else {					
					lamp_on = ON;
			}
			
			do 
			{
			
			} while (bit_is_set(PINB, PB4));
			
			}
		  sei();
		}
			
		
		
    }
}