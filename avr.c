#include "avr.h"
#include "lcd.h"

struct note{
	unsigned char freq;
	unsigned char duration;
};

void ini_avr(void)
{
	WDTCR = 15;
}

void wait_avr(unsigned short msec)
{
	TCCR0 = 3; 
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

int is_pressed( int r, int c )
{
	/* Set C to be input */
	DDRC = 0;
	
	/* Set C's outputs to all 0's to make them all 'Z', high impedance */ 
	PORTC = 0;
	
	/* Sets column to High impedance, Z*/
	CLR_BIT(PORTC,c+4);
	
	/* Sets column to Weak 1 */
	SET_BIT(PORTC,c+4);

	/* Makes the row Output mode */
	SET_BIT(DDRC, r);
	
	/* Makes the row Strong 0 */ 
	CLR_BIT(PORTC, r);

	/* If the logic output is 0, then a Strong 0 is outputted 
	   Weak 1 (a row pressed), + Strong 0 (a column is pressed) = Strong 0
	*/
	if( !(GET_BIT(PINC,c+4)) )
	{
		return 1;
	}
	return 0;
}

int get_key()
{
	for(unsigned char r = 0; r < 4; r++)
	{
		for(unsigned char c = 0; c < 4; c++)
		{
			if( is_pressed( r, c ) )
			{
				return (r*4 + c + 1);
			}
		}
	}
	return 0;
}

void play_note( unsigned char freq, unsigned char duration, unsigned char ratio )
{
	float period = ( 1.0 / ( FRQ[freq] ) );
	float th = ( period / ratio )* 10000.0 ;
	float tl;
	if ( ratio == 3 )
	{
		tl = ( (ratio-1) / ratio ) * period * 10000.0;
	}
	else if ( ratio == 2 )
	{
		tl = ( period / ratio )* 10000.0 ;
	}
	
	unsigned short limit = ( 1.0 / duration ) / period;
	for( unsigned short i = 0; i < limit; i++ )
	{
		SET_BIT( PORTB, 3 );
		wait_avr( th );
		CLR_BIT( PORTB, 3 );
		wait_avr( tl );
	}
}

int main(void)
{
	ini_lcd();
	
	char textRow0[17];
	char textRow1[17];
	
	clr_lcd();
	sprintf( textRow0, "Welcome to" );
	sprintf( textRow1, "Jimond Says" );
	puts_lcd2( textRow0 );
	pos_lcd( 1, 0 );
	puts_lcd2( textRow1 );
	
}