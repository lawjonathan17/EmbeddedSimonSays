#ifndef __avr__
#define __avr__

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>

#define XTAL_FRQ 8000000lu

#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

#define WDR() asm volatile("wdr"::)
#define NOP() asm volatile("nop"::)
#define RST() for(;;);

void ini_avr(void);
void wait_avr(unsigned short msec);
int get_key();
int is_pressed();
void play_note( unsigned char freq, unsigned char duration);
void build_combination( int* combo, unsigned short level );

#endif