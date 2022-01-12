/** @file ADC.c
 *
 * @author Runar Kjellhaug
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2005 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 19.01.06 18:12 $
 * $Revision: 4 $
 *
 */
#include <cygnal\c8051F320.h>
#include "include\ADC.h"


void ADC_Init(void)
{
  REF0CN  = 0x0e;              // Use VDD as voltage reference
  AMX0N   = 0x1f;              // ADC, Single Ended Mode
  ADC0CF  = 0x18;              // Conv. Clock bits=3, Data right-justified
}


UINT Read_Temperature(void)
{
BYTE t;
  AMX0P   = TEMP;               // Select Temp Sensor as ADC input
  for(t=0;t<20;t++);            // Small settling time
  ADC0CN  = 0x90;               // Enable ADC & initiate a conversion

  while (!(ADC0CN & ADC0INT));  // Wait for conversion Completed
  ADC0CN &= ~ADC0INT;           // Clear ADC Interrupt flag

  return ((ADC0H<<8) + ADC0L);  // Return Temperature(ADC0H & ADC0L)
}


UINT Read_RF_Vdd(void)
{
BYTE t;
  AMX0P   = RF_VDD;             // Select RF_VDD(P1.7) as ADC input
  for(t=0;t<20;t++);            // Small settling time
  ADC0CN  = 0x90;               // Enable ADC & initiate a conversion

  while (!(ADC0CN & ADC0INT));  // Wait for conversion Completed
  ADC0CN &= ~ADC0INT;           // Clear ADC Interrupt flag

  return ((ADC0H<<8) + ADC0L);  // Return RF_VDD(ADC0H & ADC0L)
}

