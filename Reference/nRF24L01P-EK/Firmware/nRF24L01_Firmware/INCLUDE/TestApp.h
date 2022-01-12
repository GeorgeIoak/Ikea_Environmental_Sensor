/** @file TestApp.h
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
 * $Revision: 4 $
 *
 */
// BYTE type definition
#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif   /* _BYTE_DEF_ */


// TestApp.c prototypes
void Enter_Testmode(BYTE testmode);
void Test_TX(void);
void Test_RX(void);
