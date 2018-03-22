#include "avr.h"
#include "lcd.h"
#include <stdlib.h>
#include <time.h>

#define A 0
#define AS 1
#define B 2
#define C 3
#define CS 4
#define D 5
#define DS 6
#define EN 7
#define F 8
#define FS 9
#define G 10
#define GS 11

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

const char* keys[16] = { "1", "2", "3", "A", "4", "5", "6", "B", "7", "8", "9","C", "Star", "0", "Pound", "D" };

struct note winMusic[7] = { {EN, H}, {EN, H}, {EN, H}, {C,H}, {C, H}, {G,H}, {GS, W} };
struct note loseMusic [12] = {{C, Q},{C, Q},{D, H},{B, H},{F, H},{F, H},{F, W},{E, W},{D, W},{C, H},{E, H},{E, H}};
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
	
	unsigned short limit = ( 1.0 / duration ) / period;
	clr_lcd();
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

void playWinMusic()
{
	for( unsigned char i = 0; i < 20; i++ )
	{
		play_note( winMusic[i].freq, winMusic[i].duration );
	}
}

void playLoseMusic()
{
	for( unsigned char i = 0; i < 12; i++ )
	{
		play_note( loseMusic[i].freq, loseMusic[i].duration );
	}
}

void playGame(int h){
	char textRow0[17];
	char textRow1[17];
 	int level = 3;
 	unsigned char key;
 	unsigned short index = 0;
 	int combo[9999];
 	build_combination( combo, level );
 	unsigned char readInput = 1;
	int highScore = h;
	int correctKeys = 0;
	for( int i = 0; i < level; i++ )
	{
		clr_lcd();
		sprintf( textRow0, "%s", keys[combo[i]]);
		play_note(combo[i], 1);
		puts_lcd2( textRow0 );
		wait_avr( 10000 );
	}
	clr_lcd();
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
					sprintf( textRow0, "%s", keys[combo[index]] );
					play_note(combo[index], 1);
					puts_lcd2( textRow0 );
					
					if( (key-1) == combo[index] )
					{
						index++;
						correctKeys++;
					}
					else
					{
						if (correctKeys > highScore){
							highScore = correctKeys;
						}
						clr_lcd();
						sprintf(textRow0, "Game Over");
						sprintf(textRow1, "High Score: %d", highScore);
						playLoseMusic();
						puts_lcd2( textRow0 );
						pos_lcd(1,0);
						puts_lcd2(textRow1);
						wait_avr(50000);
						level = 3;
						index = 0;
						correctKeys = 0;
						build_combination( combo, level );
						for( int i = 0; i < level; i++ )
						{
							clr_lcd();
							sprintf( textRow0, "%s", keys[combo[i]]);
							play_note(combo[i], 1);
							puts_lcd2( textRow0 );
							wait_avr( 10000 );
						}
						clr_lcd();
						continue;
					}
				}
			}
			
			if( index == level )
			{
				clr_lcd();
				sprintf(textRow0, "Congrats!" );
				sprintf(textRow1, "Next Level" );
				playWinMusic();
				index = 0;
				puts_lcd2(textRow0);
				pos_lcd(1,0);
				puts_lcd2(textRow1);
				wait_avr( 20000 );
				level++;
				build_combination( combo, level );
				for( int i = 0; i < level; i++ )
				{
					clr_lcd();
					sprintf( textRow0, "%s", keys[combo[i]]);
					play_note(combo[i], 1);
					puts_lcd2( textRow0 );
					wait_avr( 10000 );
				}
				readInput = 1;
			}
		}

	}
}

int main(void)
{
	ini_lcd();
	
	/* Output mode for port B */
	SET_BIT(DDRB, 3);
	int hScore = 0;

	
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
	playGame(hScore);
	

}