/** @file fwupgrade.c
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
 * $Date: 1.12.05 13:42 $
 * $Revision: 7 $
 *
 */
#include "include\fwupgrade.h"
#include "include\F320_FlashPrimitives.h"

static unsigned char xdata pageBuf[SZ_PAGE];

void WritePageBlock(unsigned char n, unsigned char *pBuf)
{
  unsigned i, pa;

  pa = SZ_BLOCK * (unsigned)n;
  for(i=0;i<SZ_BLOCK;i++)
  {
    pageBuf[pa+i] = *pBuf++;
  }
}

void ReadPageBlock(unsigned char n, unsigned char *pBuf)
{
  unsigned i, pa;

  pa = SZ_BLOCK * (unsigned)n;
  for(i=0;i<SZ_BLOCK;i++)
  {
    *pBuf++ = pageBuf[pa+i];
  }
}

void WriteFlashPage(unsigned n)
{
  unsigned i;
  unsigned pa = SZ_PAGE * n;

  FLASH_PageErase(pa);
  for (i=0;i<SZ_PAGE;i++)
  {
    FLASH_ByteWrite(pa+i, pageBuf[i]);
  }
}

void ReadFlashPage(unsigned n)
{
  unsigned i;
  unsigned pa = SZ_PAGE * n;

  for (i=0;i<SZ_PAGE;i++)
  {
    pageBuf[i] = FLASH_ByteRead(pa+i);
  }
}