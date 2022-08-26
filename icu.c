/******************************************************************************
 *
 * Module: ICU
 *
 * File Name: icu.c
 *
 * Description: Source file for the AVR ICU driver
 *
 * Author: Hisham Elsayed
 *
 *******************************************************************************/

#include "common_macros.h"
#include "HC-SR04.h"
#include "gpio.h"
#include "icu.h"       /* To use the macros like SET_BIT */
#include <avr/io.h>   /* To use registers of ICU and TIMER1 */
#include <avr/interrupt.h> /* For ICU ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_CAPT_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void ICU_init(const Icu_ConfigType * Config_Ptr)
{
	GPIO_setupPinDirection(ICU_PORT , ICP1_PIN , PIN_INPUT);    /* Configure ECHO PIN/PD6 as input pin */
    /****************************** Another method ***************************
	 * GPIO_setupPinDirection(ECHO_PORT , ECHO_PIN , PIN_INPUT);**************/

	/* Timer1 always operates in Normal Mode */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);

	/* To insert the required clock value in the first 3 bits (CS10,CS11,CS12): */
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr ->clock);

	/* To insert the required edge type in ICES1: */
	TCCR1B = (TCCR1B & 0xBF) | ((Config_Ptr -> edge)>>6);

	TCNT1 = 0;      /* Initial value of TIMER1 */
	ICR1 = 0;    /* Initial Value for the input capture register */

	/* Enable the Input Capture interrupt to generate an interrupt when edge is detected on ICP1/PD6 pin */
	TIMSK |= (1<<TICIE1);
}

void ICU_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

void ICU_setEdgeDetectionType(const Icu_EdgeType a_edgeType)
{
	/*
	 * insert the required edge type in ICES1 bit in TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0xBF) | (a_edgeType<<6);
}

uint16 ICU_getInputCaptureValue(void)
{
	return ICR1;
}

void ICU_clearTimerValue(void)
{
	TCNT1 = 0;
}


void ICU_DeInit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	ICR1 = 0;

	/* Disable the Input Capture interrupt */
	TIMSK &= ~(1<<TICIE1);
}



