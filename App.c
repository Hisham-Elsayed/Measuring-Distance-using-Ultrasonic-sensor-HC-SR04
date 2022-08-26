/*******************************************************************************
 *  [FILE NAME]: App.c
 *
 *  [Author]: Hisham Elsayed
 *
 *  [DATE CREATED]: Oct 13, 2021
 *
 *  [DESCRIPTION]: Application file for Mini Project 4
 *******************************************************************************/
#define  F_CPU 8000000    /* Define a CPU frequency of 8Mhz */
#include "HC-SR04.h"
#include "icu.h"
#include "lcd.h"
#include "gpio.h"
#include <util/delay.h>   /* For the delay functions */
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
	uint32 distance;
	/* Enable Global Interrupt (I-Bit) */
	SREG |= (1<<7);

	/* Initialize both the LCD and Ultrasonic driver */
	LCD_init();
	Ultrasonic_init();

	LCD_displayString("Distance= ");
	LCD_displayCharacter(' ');
	while (1)
	{
		LCD_moveCursor(0,13);
		distance = Ultrasonic_readDistance();
		LCD_displayString("cm");
		LCD_displayCharacter(' ');

		LCD_moveCursor(0, 10);
		if( distance >= 100 )
		{
			LCD_integerToString(distance);
		}
		else
		{
			LCD_integerToString(distance);
			/* In case the digital value is two or one digits print space in the next digit place */
			LCD_displayCharacter(' ');
		}
	}
}
