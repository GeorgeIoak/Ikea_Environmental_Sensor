/** @file FAP.h
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
 * $Revision: 9 $
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

// Protocol_API.c prototypes
void L01_Set_Channel(BYTE rf_ch);
BYTE L01_Get_Channel(void);
BYTE L01_Clear_IRQ(BYTE irq_flag);
void L01_Write_TX_Pload(BYTE *pBuf, BYTE plWidth);
BYTE L01_Get_Status(void);
BYTE L01_RD_RX_PW_n(BYTE pipe);
void L01_WR_RX_PW_n(BYTE pipe, BYTE plWidth);
BYTE L01_Get_Current_Pipenum(void);
UINT L01_Read_RX_Pload(BYTE *pBuf);
void L01_Flush_TX(void);
void L01_Flush_RX(void);
BYTE L01_Get_FIFO(void);