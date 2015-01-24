#ifndef __ANY_PHASE_CORRECT_PWM_H
#define __ANY_PHASE_CORRECT_PWM_H

#include "digitalPortsFast.h"
#include <avr/interrupt.h>

#if !defined(_useTimer0) && !defined(_useTimer1) && !defined(_useTimer2)
#define _useTimer2
#endif

static byte _pc_pwm_cnt_dir;
static byte _pc_pwm_cnt;
static byte volatile _pc_pwm[22];

#define _write_bit(by, bi, v) \
	do {\
		by = ((by) & ~_BV(bi)) | ((v) << (bi));\
	} while(0)

#define _clear_bit(by, bi) \
	do {\
		by = ((by) & ~_BV(bi));\
	} while(0)

#define _set_bit(by, bi) \
	do {\
		by |= _BV(bi);\
	} while(0)

#define _read_bit(by, bi) (((by) >> (bi)) & 1)
#define _check_bit(by, bi) ((by) & _BV(bi))

#define _analogWriteAny(P, V) \
	do {\
		_pc_pwm[(P)] = (V);\
		if((V) == 0) \
			digitalWriteFast((P), 0);\
		else if((V) == 255) \
			digitalWriteFast((P), 1);\
		} while(0)

static inline void analogWriteAny(byte pin, byte value) {
	_analogWriteAny(pin, value);
}

#ifdef ARDUINO_UNO_ALL
#define _PWM_PIN_2
#define _PWM_PIN_3
#define _PWM_PIN_4
#define _PWM_PIN_5
#define _PWM_PIN_6
#define _PWM_PIN_7
#define _PWM_PIN_8
#define _PWM_PIN_9
#define _PWM_PIN_10
#define _PWM_PIN_11
#define _PWM_PIN_12
#define _PWM_PIN_13
#endif

static inline void _pwm_timer_interrupt_handler() {
	byte cnt = _pc_pwm_cnt;
	byte set = 0;
	if (_pc_pwm_cnt_dir == 0) {
		if (cnt == 254)
			_pc_pwm_cnt_dir = 1;
#if defined(_PWM_PIN_0) || defined(_PWM_PIN_1) || defined(_PWM_PIN_2)||  defined(_PWM_PIN_3)||  defined(_PWM_PIN_4) || defined(_PWM_PIN_5) || defined(_PWM_PIN_6) || defined(_PWM_PIN_7)
		set = PORTD;
#ifdef _PWM_PIN_0
		if( _pc_pwm[0] <= cnt)
		_clear_bit(set, 0);
#endif
#ifdef _PWM_PIN_1
		if( _pc_pwm[1] <= cnt)
		_clear_bit(set, 1);
#endif
#ifdef _PWM_PIN_2
		if( _pc_pwm[2] <= cnt)
		_clear_bit(set, 2);
#endif
#ifdef _PWM_PIN_3
		if( _pc_pwm[3] <= cnt)
		_clear_bit(set, 3);
#endif
#ifdef _PWM_PIN_4
		if( _pc_pwm[4] <= cnt)
		_clear_bit(set, 4);
#endif
#ifdef _PWM_PIN_5
		if( _pc_pwm[5] <= cnt)
		_clear_bit(set, 5);
#endif
#ifdef _PWM_PIN_6
		if( _pc_pwm[6] <= cnt)
		_clear_bit(set, 6);
#endif
#ifdef _PWM_PIN_7
		if( _pc_pwm[7] <= cnt)
		_clear_bit(set, 7);
#endif
		PORTD = set;
#endif
#if defined(_PWM_PIN_8) || defined(_PWM_PIN_9) || defined(_PWM_PIN_10)||  defined(_PWM_PIN_11)||  defined(_PWM_PIN_12) || defined(_PWM_PIN_13)
		set = PORTB;
#ifdef _PWM_PIN_8
		if( _pc_pwm[8] <= cnt)
		_clear_bit(set, 0);
#endif
#ifdef _PWM_PIN_9
		if( _pc_pwm[9] <= cnt)
		_clear_bit(set, 1);
#endif
#ifdef _PWM_PIN_10
		if( _pc_pwm[10] <= cnt)
		_clear_bit(set, 2);
#endif
#ifdef _PWM_PIN_11
		if( _pc_pwm[11] <= cnt)
		_clear_bit(set, 3);
#endif
#ifdef _PWM_PIN_12
		if( _pc_pwm[12] <= cnt)
		_clear_bit(set, 4);
#endif
#ifdef _PWM_PIN_13
		if( _pc_pwm[13] <= cnt)
		_clear_bit(set, 5);
#endif
		PORTB = set;
#endif
#if defined(_PWM_PIN_14) || defined(_PWM_PIN_15) || defined(_PWM_PIN_16) || defined(_PWM_PIN_17)||  defined(_PWM_PIN_18) || defined(_PWM_PIN_19) || defined(_PWM_PIN_20)|| defined(_PWM_PIN_21)
		set = PORTC;
#ifdef _PWM_PIN_14
		if( _pc_pwm[14] <= cnt)
		_clear_bit(set, 0);
#endif
#ifdef _PWM_PIN_15
		if( _pc_pwm[15] <= cnt)
		_clear_bit(set, 1);
#endif
#ifdef _PWM_PIN_16
		if( _pc_pwm[16] <= cnt)
		_clear_bit(set, 2);
#endif
#ifdef _PWM_PIN_17
		if( _pc_pwm[17] <= cnt)
		_clear_bit(set, 3);
#endif
#ifdef _PWM_PIN_18
		if( _pc_pwm[18] <= cnt)
		_clear_bit(set, 4);
#endif
#ifdef _PWM_PIN_19
		if( _pc_pwm[19] <= cnt)
		_clear_bit(set, 5);
#endif
#ifdef _PWM_PIN_20
		if( _pc_pwm[20] <= cnt)
		_clear_bit(set, 6);
#endif
#ifdef _PWM_PIN_21
		if( _pc_pwm[21] <= cnt)
		_clear_bit(set, 7);
#endif
		PORTC = set;
#endif
		++cnt;
		_pc_pwm_cnt = cnt;
	} else {
		if (cnt == 1)
			_pc_pwm_cnt_dir = 0;
#if defined(_PWM_PIN_0) || defined(_PWM_PIN_1) || defined(_PWM_PIN_2)||  defined(_PWM_PIN_3)||  defined(_PWM_PIN_4) || defined(_PWM_PIN_5) || defined(_PWM_PIN_6) || defined(_PWM_PIN_7)
		set = PORTD;
#ifdef _PWM_PIN_0
		if( _pc_pwm[0] >= cnt)
		_set_bit(set, 0);
#endif
#ifdef _PWM_PIN_1
		if( _pc_pwm[1] >= cnt)
		_set_bit(set, 1);
#endif
#ifdef _PWM_PIN_2
		if( _pc_pwm[2] >= cnt)
		_set_bit(set, 2);
#endif
#ifdef _PWM_PIN_3
		if( _pc_pwm[3] >= cnt)
		_set_bit(set, 3);
#endif
#ifdef _PWM_PIN_4
		if( _pc_pwm[4] >= cnt)
		_set_bit(set, 4);
#endif
#ifdef _PWM_PIN_5
		if( _pc_pwm[5] >= cnt)
		_set_bit(set, 5);
#endif
#ifdef _PWM_PIN_6
		if( _pc_pwm[6] >= cnt)
		_set_bit(set, 6);
#endif
#ifdef _PWM_PIN_7
		if( _pc_pwm[7] >= cnt)
		_set_bit(set, 7);
#endif
		PORTD = set;
#endif
#if defined(_PWM_PIN_8) || defined(_PWM_PIN_9) || defined(_PWM_PIN_10)||  defined(_PWM_PIN_11)||  defined(_PWM_PIN_12) || defined(_PWM_PIN_13)
		set = PORTB;
#ifdef _PWM_PIN_8
		if( _pc_pwm[8] >= cnt)
		_set_bit(set, 0);
#endif
#ifdef _PWM_PIN_9
		if( _pc_pwm[9] >= cnt)
		_set_bit(set, 1);
#endif
#ifdef _PWM_PIN_10
		if( _pc_pwm[10] >= cnt)
		_set_bit(set, 2);
#endif
#ifdef _PWM_PIN_11
		if( _pc_pwm[11] >= cnt)
		_set_bit(set, 3);
#endif
#ifdef _PWM_PIN_12
		if( _pc_pwm[12] >= cnt)
		_set_bit(set, 4);
#endif
#ifdef _PWM_PIN_13
		if( _pc_pwm[13] >= cnt)
		_set_bit(set, 5);
#endif
		PORTB = set;
#endif
#if defined(_PWM_PIN_14) || defined(_PWM_PIN_15) || defined(_PWM_PIN_16) || defined(_PWM_PIN_17)||  defined(_PWM_PIN_18) || defined(_PWM_PIN_19) || defined(_PWM_PIN_20)|| defined(_PWM_PIN_21)
		set = PORTC;
#ifdef _PWM_PIN_14
		if( _pc_pwm[14] >= cnt)
		_set_bit(set, 0);
#endif
#ifdef _PWM_PIN_15
		if( _pc_pwm[15] >= cnt)
		_set_bit(set, 1);
#endif
#ifdef _PWM_PIN_16
		if( _pc_pwm[16] >= cnt)
		_set_bit(set, 2);
#endif
#ifdef _PWM_PIN_17
		if( _pc_pwm[17] >= cnt)
		_set_bit(set, 3);
#endif
#ifdef _PWM_PIN_18
		if( _pc_pwm[18] >= cnt)
		_set_bit(set, 4);
#endif
#ifdef _PWM_PIN_19
		if( _pc_pwm[19] >= cnt)
		_set_bit(set, 5);
#endif
#ifdef _PWM_PIN_20
		if( _pc_pwm[20] >= cnt)
		_set_bit(set, 6);
#endif
#ifdef _PWM_PIN_21
		if( _pc_pwm[21] >= cnt)
		_set_bit(set, 7);
#endif
		PORTC = set;
#endif
		--cnt;
		_pc_pwm_cnt = cnt;
	}
}

#ifdef _useTimer2
ISR(TIMER2_COMPA_vect) {
	_pwm_timer_interrupt_handler();
}
#endif

#ifdef _useTimer1
ISR(TIMER1_COMPA_vect) {
	_pwm_timer_interrupt_handler();
}
#endif

#define _setup_pin(P) do {\
		pinModeFast((P), 1);\
		_pc_pwm[(P)] = 0;\
		++total_pins; \
	} while(0)

static inline void setup_pc_pwm_any() {
	byte total_pins = 0;

	cli();
	_pc_pwm_cnt_dir = 0;
	_pc_pwm_cnt = 0;
#ifdef _PWM_PIN_0
	_setup_pin(0);
#endif
#ifdef _PWM_PIN_1
	_setup_pin(1);
#endif
#ifdef _PWM_PIN_2
	_setup_pin(2);
#endif
#ifdef _PWM_PIN_3
	_setup_pin(3);
#endif
#ifdef _PWM_PIN_4
	_setup_pin(4);
#endif
#ifdef _PWM_PIN_5
	_setup_pin(5);
#endif
#ifdef _PWM_PIN_6
	_setup_pin(6);
#endif
#ifdef _PWM_PIN_7
	_setup_pin(7);
#endif
#ifdef _PWM_PIN_8
	_setup_pin(8);
#endif
#ifdef _PWM_PIN_9
	_setup_pin(9);
#endif
#ifdef _PWM_PIN_10
	_setup_pin(10);
#endif
#ifdef _PWM_PIN_11
	_setup_pin(11);
#endif
#ifdef _PWM_PIN_12
	_setup_pin(12);
#endif
#ifdef _PWM_PIN_13
	_setup_pin(13);
#endif
#ifdef _PWM_PIN_14
	_setup_pin(14);
#endif
#ifdef _PWM_PIN_15
	_setup_pin(15);
#endif
#ifdef _PWM_PIN_16
	_setup_pin(16);
#endif
#ifdef _PWM_PIN_17
	_setup_pin(17);
#endif
#ifdef _PWM_PIN_18
	_setup_pin(18);
#endif
#ifdef _PWM_PIN_19
	_setup_pin(19);
#endif
#ifdef _PWM_PIN_20
	_setup_pin(20);
#endif
#ifdef _PWM_PIN_21
	_setup_pin(21);
#endif

	byte need_clk = ((total_pins * 8 + 44) + 8) / 16;

#ifdef _useTimer2
	TCNT2 = 0;
	TCCR2A = _BV(WGM21);
	TCCR2B = _BV(CS21) | _BV(CS20);
	OCR2A = need_clk;
	TIMSK2 |= _BV(OCIE2A);
#endif
#ifdef _useTimer1
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = _BV(WGM12)| _BV(CS11) | _BV(CS10);
	OCR1A = (need_clk +1) / 2;
	TIMSK1 |= _BV(OCIE1A);
#endif
	sei();
}

#endif /* __ANY_PHASE_CORRECT_PWM_H */
