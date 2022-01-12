/** @file USB.c
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
 * $Date: 20.02.06 14:24 $
 * $Revision: 11 $
 *
 */
#include <cygnal\c8051F320.h>
#include "include\usb.h"
#include "include\LL_API.h"

/*** [BEGIN] USB Descriptor Information [BEGIN] ***/
code const UINT USB_VID = VID_SILABS;

code const UINT USB_PID = PID_SILABS;
    //  Manufacturer String
code const BYTE USB_MfrStr[] = {0x32,0x03,'N',0,'o',0,'r',0,'d',0,'i',0,'c',0,' ',0,'S',0,'e',0,'m',0,'i',0,'c',0,'o',0,'n',0,'d',0,'u',0,'c',0,'t',0,'o',0,'r',0,' ',0,'A',0,'S',0,'A',0};             
    //  Product Desc. String
code const BYTE USB_ProductStr[]  = {0x18,0x03,'n',0,'R',0,'F',0,'2',0,'4',0,'L',0,'0',0,'1',0,'-',0,'E',0,'C',0};  
code const BYTE USB_SerialStr[]   = {0x0C,0x03,'1',0,'2',0,'3',0,'4',0,'5',0};
code const BYTE USB_MaxPower = 50;                    // Max current = 100 mA (50 * 2) (p.266 USB_20.pdf)
code const BYTE USB_PwAttributes = 0x80;              // Bus-powered, remote wakeup not supported (p.266 USB_20.pdf)
code const UINT USB_bcdDevice = FW_VERSION;           // Device firmware release number
/*** [ END ] USB Descriptor Information [ END ] ***/

//-------------------------
// Suspend_Device
//-------------------------
// Called when a DEV_SUSPEND interrupt is received.
// - Disables all unnecessary peripherals
// - Calls USB_Suspend()
// - Enables peripherals once device leaves suspend state
//
void Suspend_Device(void)
{
  // Disable peripherals before calling USB_Suspend()
  P0MDIN = 0x00;                                      // Port 0 configured as analog input
  P1MDIN = 0x00;                                      // Port 1 configured as analog input
  P2MDIN = 0x00;                                      // Port 2 configured as analog input
  P3MDIN = 0x00;                                      // Port 3 configured as analog input

  USB_Suspend();                                      // Put the device in suspend state

  // Once execution returns from USB_Suspend(), device leaves suspend state.
  // Reenable peripherals
  P0MDIN  = P0_DIN;
  P1MDIN  = P1_DIN;
  P2MDIN  = P2_DIN;
  P3MDIN  = P3_DIN;
}

void USB_Initialize(void)
{
  USB_Clock_Start();  // Init USB clock *before* calling USB_Init
  USB_Init(USB_VID,USB_PID,USB_MfrStr,USB_ProductStr,USB_SerialStr,USB_MaxPower,USB_PwAttributes,USB_bcdDevice);
  USB_Int_Enable();
}
