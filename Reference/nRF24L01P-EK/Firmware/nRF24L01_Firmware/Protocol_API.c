/** @file Protocol_API.c
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
 * $Revision: 10 $
 *
 */
//  Global defines & includes
#include <string.h>
#include "include\Protocol_API.h"
#include "include\nRF_API.h"
#include "include\defines.h"

void L01_Set_Channel(BYTE rf_ch)                                // Set RF channel
{
  SPI_RW_Reg(WRITE_REG + RF_CH,rf_ch);
}

BYTE L01_Get_Channel(void)                                      // Get current RF channel
{
  return SPI_Read(RF_CH);
}

BYTE L01_Clear_IRQ(BYTE irq_flag)                               // Clear nRF24L01 IRQ flag(s)
{
  return SPI_RW_Reg(WRITE_REG + STATUS, irq_flag);
}

void L01_Write_TX_Pload(BYTE *pBuf, BYTE plWidth)               // Write TX payload, payload in *pBuf & #of bytes = plWidth
{
  SPI_Write_Buf(WR_TX_PLOAD, pBuf, plWidth);
}

BYTE L01_Get_Status(void)                                       // Read status byte
{
  return SPI_Read(STATUS);
}

BYTE L01_RD_RX_PW_n(BYTE pipe)                                  // Get current RX payload width for pipe.n
{
  return SPI_Read(RX_PW_P0 + pipe);
}

void L01_WR_RX_PW_n(BYTE pipe, BYTE plWidth)                    // Set RX payload width for pipe.n
{
  SPI_RW_Reg(WRITE_REG + RX_PW_P0 + pipe, plWidth);
}


BYTE L01_Get_Current_Pipenum(void)                              // Get current pipe#
{
  return ((L01_Get_Status() & RX_P_NO) >> 1);
}

UINT L01_Read_RX_Pload(BYTE *pBuf)                              // read current pipe#'s RX payload
{
BYTE plWidth, pipe;
  plWidth = L01_RD_RX_PW_n(pipe = L01_Get_Current_Pipenum());   // Read current pipe's payload width
  SPI_Read_Buf(RD_RX_PLOAD, pBuf, plWidth);                     // Then get RX data

  return ((pipe << 8) + plWidth);                               // return pipe# & pipe#.plWidth
}

void L01_Flush_TX(void)                                         // Flush TX FIFO
{
  SPI_RW_Reg(FLUSH_TX,0);
}

void L01_Flush_RX(void)                                         // Flush RX FIFO
{
  SPI_RW_Reg(FLUSH_RX,0);
}

BYTE L01_Get_FIFO(void)                                         // Read FIFO_STATUS register
{
  return SPI_Read(FIFO_STATUS);
}