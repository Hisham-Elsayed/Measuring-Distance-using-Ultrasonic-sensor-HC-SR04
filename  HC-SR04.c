/******************************************************************************
 *
 * Module: HC-SR04
 *
 * File Name:  HC-SR04.c
 *
 * Description: Source file for the AVR HC-SR04 Ultrasonic driver
 *
 * Author: Hisham Elsayed
 *
 *******************************************************************************/

#include "HC-SR04.h"
#include "icu.h"
#include "common_macros.h" /* To use macros like SET_BIT */
#include "gpio.h"
#include <util/delay.h>   /* For the delay functions */
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
uint8  g_edgeCount = 0;
uint16 g_timeHigh = 0;
uint32 distance = 0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void Ultrasonic_init(void)
{
	/* Create configuration structure for ICU driver */
	Icu_ConfigType Icu_Config = {F_CPU_8,RISING};

	/* Initialize the ICU driver */
	ICU_init(&Icu_Config);

	/* Setup the ICU call back function  */
	ICU_setCallBack(Ultrasonic_edgeProcessing);

	/* Setup the direction for the trigger pin as output pin */
	GPIO_setupPinDirection(TRIGGER_PORT , TRIGGER_PIN , PIN_OUTPUT );
}

void Ultrasonic_Trigger(void)
{
	/* Send the trigger pulse to the ultrasonic */
	GPIO_writePin(TRIGGER_PORT, TRIGGER_PIN,LOGIC_HIGH );
	_delay_us(20);   /* We need to transmit trigger pulse of at least 10 us */
	GPIO_writePin(TRIGGER_PORT, TRIGGER_PIN, LOGIC_LOW);
}

uint16 Ultrasonic_readDistance(void)
{

	Ultrasonic_Trigger();    /* Send the trigger pulse */

	if(g_edgeCount == 2)
	{
		g_edgeCount = 0;
		/* Calculate the measured distance in cm */
		distance=(float)(g_timeHigh/57.5);

		/* To prevent ultrasonic reading to be different from the LCD reading: */
		if (distance < 22)
		{
			distance++;
		}
		if (distance >= 137)
		{
			distance--;
		}
		if (distance >= 254)
		{
			distance--;
		}
		if (distance >= 367)
		{
			distance--;
		}
	}

	return distance;
}


void Ultrasonic_edgeProcessing(void)
{
	g_edgeCount++;
	if(g_edgeCount == 1)
	{
		/*
		 * Clear the timer counter register to start measurements from the
		 * first detected rising edge
		 */
		ICU_clearTimerValue();
		/* Detect falling edge */
		ICU_setEdgeDetectionType(FALLING);
	}
	else if(g_edgeCount == 2)
	{
		/* Store the High time value */
		g_timeHigh = ICU_getInputCaptureValue();
		/* Detect rising edge */
		ICU_setEdgeDetectionType(RISING);
	}
}


