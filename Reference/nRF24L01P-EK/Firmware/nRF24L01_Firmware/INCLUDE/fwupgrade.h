/** @file fwupgrade.h
 *
 * @author Ole Saether
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
 * $Date: 28.11.05 10:34 $
 * $Revision: 4 $
 *
 */
#define SZ_BLOCK 32
#define SZ_PAGE 512

void WritePageBlock(unsigned char n, unsigned char *pBuf);
void ReadPageBlock(unsigned char n, unsigned char *pBuf);
void WriteFlashPage(unsigned n);
void ReadFlashPage(unsigned n);
