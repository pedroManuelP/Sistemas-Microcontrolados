#ifndef _DEFS_PRINCIPAIS_AVR_H
#define _DEFS_PRINCIPAIS_AVR_H

#ifndef F_CPU
#define F_CPU 16000000UL //define clock do ATMega328P
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y,bit_X) (Y|=(1<<bit_X)) // set num bit
#define clr_bit(Y,bit_X) (Y&=~(1<<bit_X)) // clear num bit
#define cpl_bit(Y,bit_X) (Y^=(1<<bit_X)) // inverte um bit
#define tst_bit(Y,bit_X) (Y&(1<<bit_X)) // testa um bit(isola um bit)

#endif
