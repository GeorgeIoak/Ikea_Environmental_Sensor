/** @file ADC.h
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
 * $Date: 20.02.06 14:16 $
 * $Revision: 5 $
 *
 */
// UINT type definition
#ifndef _UINT_DEF_
#define _UINT_DEF_
typedef unsigned int UINT;
#endif  /* _UINT_DEF_ */

// BYTE type definition
#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif   /* _BYTE_DEF_ */

// Define ADC sources
#define TEMP    0x1e
#define	RF_VDD	0x07
#define ADC0INT 0x20



// ADC.h prototypes
void ADC_Init(void);
UINT Read_RF_Vdd(void);
UINT Read_Temperature(void);
