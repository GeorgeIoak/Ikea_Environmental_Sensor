/** @file TestApp.c
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
 * $Date: 2.03.06 15:25 $
 * $Revision: 9 $
 *
 */
#include <cygnal\c8051F320.h>
#include "include\TestApp.h"
#include "include\defines.h"
#include "include\nRF_API.h"
#include "include\LL_API.h"

extern BYTE Timer_Mode;
extern BYTE Test_Mode;
extern BYTE IRQ_Source;

#define TX_AW     5
code const BYTE TX_ADDRESS__[TX_AW] = {0x40,0x50,0x60,0x70,0x80};                 // Predefined TX_Address

#ifdef _TESTMODE_
#define RX_AW     5
#define TX_PL_W   32
code const BYTE RX_ADDRESS_[RX_AW]   = {0x40,0x50,0x60,0x70,0x80};                // Predefined RX_Address
code const BYTE TX_PAYLOAD_[TX_PL_W] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,  // Predefined
                                        0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,  // TX payload
                                        0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
                                        0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70};
#endif

void Enter_Testmode(BYTE testmode)
{
  ET2 = 0;                                                                        // T2 Interrupt enabled
  EX0 = 0;                                                                        // Disable External Interrupt0
  CE = 0;                                                                     
  Led1 = Led2 = Led3 = Led4 = 1;                                                  // Clear status Led's

  if(testmode)                                                                    // Entered Test Mode
  {
    Timer_Mode = 0;
    Test_Mode = 1;
    Start_T2();
  }
  else                                                                            // Exit Test Mode
  {
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x08);                                         // Config reg, default reset value
    SPI_RW_Reg(WRITE_REG + EN_AA, 0x3f);                                          // Default, Auto ack all pipe's
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x03);                                      // Default, pipe 0 & 1 enabled
    SPI_RW_Reg(WRITE_REG + SETUP_AW, 0x03);                                       // Default, 5 bytes address
    SPI_RW_Reg(WRITE_REG + STATUS, MASK_IRQ_FLAGS);                               // Clear all IRQ flag
    SPI_RW_Reg(FLUSH_TX,0);                                                       // Just in case, flush TX FIFO
    SPI_RW_Reg(FLUSH_RX,0);                                                       // and RX FIFO
    SPI_Write_Buf(WRITE_REG + TX_ADDR,TX_ADDRESS__,TX_AW);                        // Writes TX_Addr
    Test_Mode = 0;                                                                // Exit testmode
    IRQ_Source = 0;                                                               // Reset pending interrupt
    Stop_T2();
  }
}



#ifdef _TESTMODE_
//************************************************
//
// Default Test_TX function
// for "production test"
// 
//
//************************************************
void Test_TX(void)
{
  Led1 = Led2 = Led3 = Led4 = 1;                                                  // Clear status Led's

  SPI_RW_Reg(WRITE_REG + SETUP_AW, (TX_AW-2));                                    // Setup RX/TX address width
  SPI_Write_Buf(WRITE_REG + TX_ADDR,TX_ADDRESS_,TX_AW);                           // Writes TX_Addr
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS_,RX_AW);                       // Writes RX_Addr, Pipe0 for auto.ack

  SPI_Write_Buf(WR_TX_PLOAD,TX_PAYLOAD_,TX_PL_W);                                 // Writes TX payload

  SPI_RW_Reg(WRITE_REG + STATUS, 0x70);                                           // Read status & clear IRQ flag's
  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);                                            // Enable auto ack pipe0
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);                                        // Enable pipe0
  SPI_RW_Reg(WRITE_REG + SETUP_RETR,0x0a);                                        // 250탎+86탎, 10 retrans...
  SPI_RW_Reg(WRITE_REG + RF_CH, 40);                                              // Channel 40

  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);                                           // PWR_UP, CRC(2bytes) & Prim:TX.

  while(!SW1);                                                                    // wait until SW1 released
  Led1 = 0;                                                                       // Led1 indicates TX mode
  EX0 = 1;                                                                        // Enable External Interrupt0
}

//************************************************
//
// Default Test_RX function
// for "production test"
// 
//
//************************************************
void Test_RX(void)
{
  Led1 = Led2 = Led3 = Led4 = 1;                                                  // Clear status Led's
  
  SPI_RW_Reg(WRITE_REG + SETUP_AW, (TX_AW-2));                                    // Setup RX/TX address width
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS_,RX_AW);                       // Writes RX_Addr

  SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PL_W);                                      // Pipe0 payload width
  SPI_RW_Reg(WRITE_REG + STATUS, 0x70);                                           // read status & clear IRQ flag's
  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);                                            // Enable auto ack pipe0
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);                                        // Enable pipe0
  SPI_RW_Reg(WRITE_REG + SETUP_RETR,0x0a);                                        // 250탎+86탎, 10 retrans...
  SPI_RW_Reg(WRITE_REG + RF_CH, 40);                                              // Channel 40

  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);                                           // PWR_UP, CRC(2bytes) & Prim:RX.

  while(!SW2);                                                                    // Wait until SW2 released
  Led2 = 0;                                                                       // Led2 indicates RX mode
  EX0 = 1;                                                                        // Enable External Interrupt0
}
#endif
