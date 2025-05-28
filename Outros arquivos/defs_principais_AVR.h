#ifndef _DEFS_PRINCIPAIS_AVR_H
#define _DEFS_PRINCIPAIS_AVR_H

#ifndef F_CPU
#define F_CPU 16000000UL //define clock do ATMega328P
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef set_bit(Y,bit_X)
#define set_bit(Y,bit_X) (Y|=(1<<bit_X)) // set num bit
#endif
#ifndef clr_bit(Y,bit_X)
#define clr_bit(Y,bit_X) (Y&=~(1<<bit_X)) // clear num bit
#endif
#ifndef cpl_bit(Y,bit_X)
#define cpl_bit(Y,bit_X) (Y^=(1<<bit_X)) // inverte um bit
#endif
#ifndef tst_bit(Y,bit_X)
#define tst_bit(Y,bit_X) (Y&(1<<bit_X)) // testa um bit(isola um bit)
#endif
  
#endif
