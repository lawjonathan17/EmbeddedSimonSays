#include "avr.h"
#include "lcd.h"
#include <stdlib.h>
#include <time.h>

#define W 1
#define H 2
#define Q 3
#define E 4

struct note{
	unsigned char freq;
	unsigned char duration;
};

const float FRQ[16] = { 220.000, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628,
						349.228, 369.994, 391.995, 415.305, 440.000, 466.164, 493.883, 523.251 };

const char* keys[16] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "Star", "Pound" };

void ini_avr(void)
{
	WDTCR = 15;
}

void wait_avr(unsigned short msec)
{
	TCCR0 = 2; 
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.0001);
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

void play_note( unsigned char freq, unsigned char duration )
{
	char testRow0[17];
	
	float period = ( 1.0 / ( FRQ[freq] ) );
	float th,tl = ( period / 2 ) * 10000.0 ;
	
	// unsigned short limit = ( 1.0 / duration ) / period;
	unsigned short limit = ( 1.0 / duration ) / period;
	clr_lcd();
	sprintf( testRow0, "%d", limit );
	puts_lcd2( testRow0 );
	for( unsigned short i = 0; i < limit; i++ )
	{
		SET_BIT( PORTB, 3 );
		wait_avr( th );
		CLR_BIT( PORTB, 3 );
		wait_avr( tl );
	}
}

void build_combination( int* combo, unsigned short level )
{
//	char textRow0[17];
	for( unsigned short i = 0; i < level; i++ )
	{
		unsigned short keysIndex = ( rand() % 16 );
		combo[i] = keysIndex;
//		sprintf( textRow0, "%s", keys[keysIndex] );
//		puts_lcd2( textRow0 );
//		wait_avr( 10000 );
//		clr_lcd();
	}
}

void buildUserInput( int* userInput, int level )
{
	unsigned short index = 0;
	unsigned short key;
	char textRow0[17];
	unsigned short run = 1;
	while( run )
	{
		if( index == level )
		{
			run = 0;
		}
		for( unsigned char i = 0; i < 4; i++ )
		{
			wait_avr(250);
			key = get_key();
			if( key )
			{
				clr_lcd();
				sprintf( textRow0, "in: %d, k: %d", index, key );
				puts_lcd2( textRow0 );
				userInput[index++] = key;
			}
		}
	}
}

int main(void)
{
	ini_lcd();
	
	/* Output mode for port B */
	SET_BIT(DDRB, 3);
	
	char textRow0[17];
	char textRow1[17];
	
// 	/* Welcome screen */
 	clr_lcd();
// 	sprintf( textRow0, "Welcome to" );
// 	sprintf( textRow1, "Jonmond Says" );
// 	puts_lcd2( textRow0 );
// 	pos_lcd( 1, 0 );
// 	puts_lcd2( textRow1 );
// 	
// 	/* Instructions */
//  	wait_avr( 3000 );
//  	clr_lcd();
//  	sprintf( textRow0, "Test your memory" );
// 	sprintf( textRow1, "Match the keys" );
//  	puts_lcd2( textRow0 );
// 	pos_lcd( 1, 0 );
// 	puts_lcd2( textRow1 );
// 	wait_avr( 4000 );
// 	clr_lcd();

	srand(time(NULL));
// 	int a = ( rand() % 200 ) + 1;
// 	clr_lcd();
// 	sprintf( textRow0, "%d", a );
// 	puts_lcd2( textRow0 );
// 	a = 0;
 	int level = 3;
	unsigned char key;
	unsigned short index = 0;
	int userInput[level];
	int combo[level];
	build_combination( combo, level );
	unsigned char readInput = 1;
	
	for( int i = 0; i < level; i++ )
	{
		clr_lcd();
		sprintf( textRow0, "c: %d , i: %d", combo[i], i);
		puts_lcd2( textRow0 );
		wait_avr( 10000 );
	}
	clr_lcd();
	
	sprintf( textRow0, "c = %d", combo[0] );
	puts_lcd2(textRow0);
	while( 1 )
	{
		if( readInput )
		{
			for( unsigned char i = 0; i < 4; i++ )
			{
				wait_avr(2500);
				key = get_key();
				
				if(key)
				{
					clr_lcd();
					// sprintf( textRow0, "k = %d, c: %d", key-1, *(combo + index) );
					sprintf( textRow0, "k = %d", key-1);
					sprintf( textRow1, "c = %d", combo[index] );
					puts_lcd2( textRow0 );
					pos_lcd(1, 0);
					puts_lcd2(textRow1);
					
					if( (key-1) == combo[index] )
					{
						index++;
					}
					else
					{
						//clr_lcd();
						//sprintf(textRow0, "wrong");
						//puts_lcd2( textRow0 );
					}
					
				}
			}
		}

		
		
		
// 		for( int i = 0; i < level; i++ )
// 		{
// 			clr_lcd();
// 			sprintf( textRow0, "c: %d , i: %d",  *(combo + i), i);
// 			puts_lcd2( textRow0 );
// 			wait_avr( 10000 );
// 		}
		
// 		for( unsigned char i = 0; i < 4; i++ )
// 		{
// 			wait_avr(2500);
// 			key = get_key();
// 			if( key )
// 			{
// 				clr_lcd();
// 				//if( key == combo[index] )
// 				//{
// 					sprintf( textRow0, "key = %d", key );
// 					puts_lcd2( textRow0 );
// 					wait_avr( 20000 );
// 					//index++; 
// 				//}
// 				//else
// 				//{
// 					//sprintf( textRow0, "fail" );
// 					//puts_lcd2( textRow0 );
// 				//}
// // 				clr_lcd();
// // 				sprintf( textRow0, "in: %d, k: %d", index, key );
// // 				puts_lcd2( textRow0 );
// // 				userInput[index++] = key;
// 			}
// 		}
		
// 		clr_lcd();
// 		buildUserInput( userInput, level );
// 		sprintf( textRow0, "test" );
// 		puts_lcd2( textRow0 );
// 		while(1);
		/*
		for( int i = 0; i < level; i++ )
		{
			clr_lcd();
			sprintf( textRow0, "u: %d , i: %d",  userInput[i], i);
			puts_lcd2( textRow0 );
			wait_avr( 10000 );
		}
		*/
		// b_c() return the array
		// Build an array of user input
		// Check the user input array
		// Success / Failure results
	}
}