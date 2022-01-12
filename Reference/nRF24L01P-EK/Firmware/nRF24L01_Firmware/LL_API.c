/** @file low_level.c
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
 * $Date: 2.03.06 15:23 $
 * $Revision: 41 $
 *
 */
#include <cygnal\c8051F320.h>
#include "include\LL_API.h"
#include "include\usb.h"
#include "include\nRF_API.h"
#include "include\ADC.h"
#include "include\Protocol_API.h"
#include "include\defines.h"

extern BYTE xdata TX_pload[TX_PLOAD_MAX_WIDTH];         // TX payload buffer
extern BYTE xdata Freq_table[FREQ_TABLE_SIZE];          // Freq Agility table
extern BYTE xdata Freq_agil[NUM_OF_PIPES];              // Freq Agility On/Off for pipe.n
extern BYTE xdata Trans_Ctrl[TRANS_PARAMS];             // Transmission Control bytes
extern BYTE xdata RX_pload_length[NUM_OF_PIPES];        // holds #of bytes for pipe 0..5
extern BYTE xdata TX_pload_width;                       // TX payload width variable
extern BYTE xdata TX_buffer[USB_TX_SIZE];               // Next packet to sent to host
extern BYTE LinkStatus[2];
extern BYTE LinkStat, LastStat;                         // Link Status bytes
extern BYTE xdata Button, SW1_Released;
extern BYTE xdata Button_Mode;

extern BYTE Com_Mode;
extern BYTE SPI_Mode;
extern BYTE Led_Blink1,Led_Blink2;
extern BYTE Led_Blink3,Led_Blink4;
extern UINT Link_Loss_Delay;
extern BYTE Try_Ctr;

code const BYTE P0_ADDR[4] =  {0xE7,0xE7,0xE7,0xE7};    // Default P0 address
code const BYTE P1_ADDR[4] =  {0xC2,0xC2,0xC2,0xC2};    // Default P1 address

void CPU_Init(void)
{
BYTE counter;

  PCA0MD &= ~0x40;                                      // Disable Watchdog timer
  Port_Init();
  SPI_Init(HW_MODE);                                    // Default HW SPI Mode
  ADC_Init();                                           // Init & enable ADC
  OSCICN |= (0x03);                                     // SYSCLK = OSC/1, i.e. 12MHz
  IT01CF = 0x05;                                        // Assign /INT0 to P0.5 pin
  IT0 = 1;                                              // Edge trigged interrupt0 (nRF24L01 IRQ)
  
  Init_T0();                                            // Init Timer0..
  Start_T1();                                           // Init Timer1
  Init_T2();                                            // Start Timer2
  
  USB_Initialize();
  REG0CN |= 0x80;                                       // Internal Voltage Regulator Disabled
  
  TX_pload_width = 16;
  for(counter=0;counter<TX_pload_width;counter++) 
    TX_pload[counter] = counter+2;                      // Init TX_payload buffer
  
  Freq_agil[PIPE0] = 8;                                 // Init Freq_agil for pipe0; 8ms
  for(counter=1;counter<6;counter++)                    
    Freq_agil[counter] = 0;                             // the rest is disabled
  
  for(counter=0;counter<6;counter++)                    // Init RX_pload_length for all pipe's
    RX_pload_length[counter] = 0x10;                    // Default 16 bytes
  
  Trans_Ctrl[0] = 0;                                    // Default trans ctrl; Timer Mode
  Trans_Ctrl[1] = 5;                                    // Default timer: 5ms
  Com_Mode = 2;                                         // Default, communication mode disabled
}

//-------------------------
// Port_Init
//-------------------------
// Port Initialization
// - Configure the Crossbar and GPIO ports.
//
void Port_Init(void)
{

  P0MDOUT = P0_DOUT;                                    // Port0..3 Mode & Values, see. "low_level.h" for details
  P1MDOUT = P1_DOUT;
  P2MDOUT = P2_DOUT;
  P2MDOUT = P2_DOUT;

  P0MDIN  = P0_DIN;
  P1MDIN  = P1_DIN;
  P2MDIN  = P2_DIN;
  P3MDIN  = P3_DIN;

  P0      = P0_INIT;
  P1      = P1_INIT;
  P2      = P2_INIT;
  P3      = P3_INIT;

  XBR0    = XBAR0;                                      // XBR0.1 => SPI Enabled
  XBR1    = XBAR1;                                      // Enable Crossbar
}


void WD_Reset(void)                                     // "Reset" µC
{
  USB_Disable();
  ET2 = 0x00;
  EX0 = 0x00;
  PCA0CPL4  = 0xff;
  PCA0MD    = 0x00;
  PCA0MD   |= 0x40;

  while(1);
}


void Init_T0(void)
{
  TH0 = 253;                                            // Init T0 reload value, 12µs
  TL0 = 253;                                            // Init T0 start value..
  CKCON |= 0x02;                                        // T0 = SCK/48
  TMOD |= 0x02;                                         // T0:Mode2, 8-bit reload timer.
  ET0 = 1;                                              // Enable timer0 interrupt
}

void Start_T1(void)
{
  TH1 = 194;                                            // Init T1 reload value, 250µs
  TL1 = 194;                                            // Init T1 start value..
  CKCON |= 0x02;                                        // T1 = SCK/48
  TMOD |= 0x20;                                         // T1:Mode2, 8-bit reload timer.
  ET1 = 1;                                              // Enable timer1 interrupt
  TR1 = 1;                                              // Start timer1...
}


void Init_T2(void)                                     // Init Timer2 for 1ms intervals
{
  TMR2H = 0x00;                                         // T2 CounterH = 0
  TMR2L = 0x00;                                         // T2 CounterL = 0
  TMR2RLH = 0xfc;                                       // T2 ReloadH
  TMR2RLL = 0x18;                                       // T2 ReloadL
//  TMR2CN  = 0x04;                                       // T2 Enabled
}

void Start_T2()                                         // Start Timer2
{
  TMR2CN |= 0x04;                                       
}

void Stop_T2()                                          // Stop Timer2 and clear T2 counter
{
  TMR2CN &= ~0x04;
//  TMR2H = 0x00;                                         // T2 CounterH = 0
//  TMR2L = 0x00;                                         // T2 CounterL = 0

}

void Write_Led(BYTE led, BYTE state)                    // Set/reset LEDn
{
  switch(led)
  {
    case LED1:
      Led1 = !state;                                    // Output value to LED1
    break;

    case LED2:
      Led2 = !state;                                    // Output value to LED2
    break;

    case LED3:
      Led3 = !state;                                    // Output value to LED3
    break;

    case LED4:
      Led4 = !state;                                    // Output value to LED4
    break;
  }

}

void Toggle_Led(BYTE led)                               // Toggle state of LEDn
{
  switch(led)
  {
    case LED1:
      Led1 = ~Led1;
    break;
    
    case LED2:
      Led2 = ~Led2;
    break;
    
    case LED3:
      Led3 = ~Led3;
    break;
    
    case LED4:
      Led4 = ~Led4;
    break;
  }
}

void Blink_Led(BYTE led)                                // Lit LEDn, timer1 clear LEDn
{
  switch(led)
  {
    case LED1:
      Led1 = 0;
      Led_Blink1 = 2;
    break;

    case LED2:
      Led2 = 0;
      Led_Blink2 = 2;
    break;

    case LED3:
      Led3 = 0;
      Led_Blink3 = 2;
    break;

    case LED4:
      Led4 = 0;
      Led_Blink4 = 2;
    break;
  }

}

void CE_Pin(BYTE action)                                // CE pin high, low or pulse..
{
  switch(action)
  {
    case CE_LOW:                                        // action == 0, CE low
      CE = 0;
    break;

    case CE_HIGH:                                       // action == 1, CE high
      CE = 1;
    break;

    case CE_PULSE:                                      // action == 2, CE pulse (10µs)
      Blink_Led(LED2);
      CE = 1;                                           // Set CE pin high
      TR0 = 1;                                          // Start Timer0, CE pulse timer
    break;
  }
}

void CSN_Pin(BYTE state)                                // Set/reset CSN pin
{
  if(state)
    CSN = 1;
  else
    CSN = 0;
}

void SCK_Pin(BYTE state)                                // Set/reset SCK pin
{
  if(state)
    SCK = 1;
  else
    SCK = 0;
}

void MOSI_Pin(BYTE state)                               // Set/reset MOSI pin
{
  if(state)
    MOSI = 1;
  else
    MOSI = 0;
}

BYTE MISO_Pin(void)                                     // Read MISO pin
{
  return MISO;
}

BYTE SPI_HW(BYTE byte)                                  // Write one byte using F320's hardware SPI
{
  SPI0DAT = byte;                                       // Writes 'byte' to nRF24L01
  while(WAIT_SPIF);                                     // ..and wait until SPIF goes high, i.e. transaction finish
  SPI0CN &= 0x7f;                                       // clear SPIF

  return(SPI0DAT);                                      // return received byte
}

void SPI_Init(BYTE Mode)
{
  if(Mode)                                             // If HW_SW = 1 =>> hardware SPI, else software SPI.
  {
    SPI0CFG = SPI_CFG;                                  // SPI Configuration Register
    SPI0CN  = SPI_CTR;                                  // SPI Control Register
    SPI0CKR = SPI_CLK;                                  // SPI Clock Rate Register
    XBR0 |= SPI0E;                                      // SPI SW Mode, enable SPI Crossbar

    SPI_Mode = HW_MODE;
  }
  else                                                  // Software SPI mode
  {
    SPI0CN  = 0x00;                                     // SPI Control Register, SPI Disabled
    
    XBR0 &= ~SPI0E;                                     // SPI SW Mode, disable SPI Crossbar
    SPI_Mode = SW_MODE;
  }
}

void Update_Link_Status(void)
{
  if(LinkStat != LastStat)                              // Same Link Status as last time?
  {
    TX_buffer[0] = LinkStatus[LINK_STATUS];
    TX_buffer[1] = LinkStatus[LINK_CHANNEL];
    TX_buffer[2] = Link_Loss_Delay>>8;
    TX_buffer[3] = Link_Loss_Delay;
    Block_Write(TX_buffer, 4);
  }
  LastStat = LinkStat;
}


BYTE Send_Packet_Button(void)                           // Send one data packet, controlled by Button1(SW1)
{
BYTE send_packet;
  if(!SW1)
  {                                                     // Button1 pressed?
    if(SW1_Released)                                    // and Button1 released since last press?
    {
      SW1_Released = 0;                                 // Button1 "pressed"
      send_packet = 1;                                  // flag for packet sending..
    }
    else
    {
      send_packet = 0;                                  // dont send packet
    }
  }
  else
  {
    SW1_Released = 1;                                   // Button1
    send_packet = 0;                                    // dont send packet
  }

  if(send_packet)
  {
    L01_Write_TX_Pload(TX_pload, TX_pload_width);       // Write new TX payload
    CE_Pin(CE_PULSE);                                   // Enable CE pulse, 12µs
    Try_Ctr = 0;                                        // Reset Try_Ctr before nex transmitt..
  }

  return send_packet;                                   // packet sent or not?

}

void Check_Button_TX(void)
{
  if(Button_Mode && (Com_Mode == TX_MODE))              // Scan button1 ONLY if "TX_MODE" AND Button_Mode
  {
    switch(LinkStat)
    {
      case LINK_ESTABLISH:
        if(Send_Packet_Button())                        // Blink LED2 and set new LinkStat if packet sent
        {
          Blink_Led(LED2);
          Try_Ctr = 0;
          LinkStat = LINK_LOSS;                         // default LINK_LOSS
        }
      break;
      
      default:
      
      break;
    }

  }

}