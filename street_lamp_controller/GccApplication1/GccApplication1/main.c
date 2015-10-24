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


volatile unsigned int time_count;
volatile uint8_t lamp_on = OFF, lamp_state = OFF;
volatile uint8_t check_press = 0, check_unpress=0;
volatile uint8_t in_data = 0;
volatile uint8_t in_data_tmp = 0;
volatile uint8_t led_flash = 0;
volatile unsigned char out_data;

ISR(TIMER0_COMPA_vect){	
	if (!(lamp_state & 1)) 
	{
		 lamp_state = OFF;
		 led_flash = OFF;
	}
	
	if ((time_count > 0) & (lamp_state == 0b10100001)) {
			led_flash = led_flash ^ 1;
			time_count--;
			if (time_count == 0) {
				lamp_state = OFF;
		}
	} else
	{
		time_count = 0;
		
	}	
	
	
	if (lamp_state & 0b00000001)	{
		out_data = 0b00011110;				
	} else
	{				
		out_data = 0b00011111;
	}
	//led_flash = in_data;
	if (led_flash) {
	  out_data |= (1<<5);
	} else
	{
	  out_data &=~ (1<<5);		
	}
	
	PORTB = out_data;	
}

	

void lamp_on_delay() {
	lamp_on = ON | (1<<7) | (1<<6) | (1<<5);	
};

void lamp_on_perm() {
	if (lamp_state) {
		lamp_on = OFF | (1<<7) | (1<<6);
	} else
	{
		lamp_on = ON | (1<<7) | (1<<6);
	}
	time_count = 0;
}

void set_lamp_state(uint8_t button) {
	if ((button & (1<<PB1)) | ((button & (1<<PB4)))) {
		lamp_on_delay();
	} else
	if ((button & (1<<PB2)) | ((button & (1<<PB3)))) {
		lamp_on_perm();
	}
}


void control() {
	in_data = (PINB ^ ~((1<<PB3)|(1<<PB1))) & 0b00011110;
	if (in_data) {
		if (!(0b11000000 & lamp_on)) {
			if (in_data == in_data_tmp)	{
				check_press++;
				if (check_press > 10) {
					set_lamp_state(in_data);
				}
				return;
			}
			in_data_tmp = in_data;
			check_press = 0;
		}
		check_unpress = 0;
		return;
	}
	check_press = 0;
	if (lamp_on & (1<<6)) {
		if (check_unpress > 10) {
			lamp_on &=~(1<<6);
			if (lamp_on & (1<<5)) {
				time_count += TIME_DELAY;
			}
			lamp_state = lamp_on;
			lamp_on = 0;
		}
		check_unpress++;
		return;
	}	
}

ISR (TIMER1_COMPA_vect) {
	control();  
}



int main(void)
{
	PORTB = 0b11111111; // на pin0 0, на остальных включена PullUp
	DDRB = 0b00100001; //pin0 выход, остальные вход
	time_count = 0;
	cli();
	TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00); // делитель 1024
	TCCR0A |= (1 << WGM01); // переполнение
	
	OCR0A = 125; // счетчик переполнения
	TCCR1 = (0<< CTC1) | (0 << PWM1A) | (0<<COM1A1) | (0<<COM1A0) | (0<<CS13) | (0<<CS12) | (1<<CS11) | (1<<CS10);
	OCR1A = 100;
	TIMSK |= (1 << OCIE0A) | (1<<OCIE1A); //	

	lamp_on = OFF;
    sei();
    while(1)
    {
		
    }
}