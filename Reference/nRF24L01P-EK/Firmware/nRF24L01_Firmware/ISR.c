/** @file ISR.c
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
 * $Revision: 22 $
 *
 */
//  Global defines & includes
#include <cygnal\c8051F320.h>
#include "include\usb.h"
#include "include\defines.h"
#include "include\nRF_API.h"
#include "include\LL_API.h"
#include "include\Protocol_API.h"

extern BYTE xdata RX_buffer[USB_RX_SIZE];
extern BYTE xdata RX_pload[RX_PLOAD_MAX_WIDTH];
extern BYTE xdata TX_pload[TX_PLOAD_MAX_WIDTH];
extern BYTE xdata TX_pload_width;
extern BYTE USB_Rx_Comp;
extern BYTE IRQ_Source;
extern BYTE Timer_Mode;
extern BYTE Trans_Tmr,Trans_Tmr_Ctr;
extern BYTE Sweep_Mode;
extern BYTE Sweep_Time_Ctr;
extern BYTE sw_low,sw_high,next_ch;
extern BYTE Sweep_Progress;
//extern BYTE RX_pipe_num;
extern BYTE Test_Mode;
extern BYTE Com_Mode;
extern BYTE xdata Table_Ptr;                                      // Pointer for Freq_table lookup
extern BYTE xdata Freq_table[FREQ_TABLE_SIZE];
extern BYTE Agil_Timeout;
extern BYTE LinkStatus[2];
extern BYTE LinkStat, LastStat;
extern BYTE TX_Update;
extern BYTE Led_Blink1,Led_Blink2,Led_Blink3,Led_Blink4;
extern UINT Link_Loss_Delay;
//****************************************
//
// Extern Interrupt0
//
// Interrupt from nRF24L01
// data ready in RX-payload
//
// In _TESTMODE_:
// Read RX-payload, clear RX_DR....
//
//****************************************
void nRF24L01_IRQ(void) interrupt EXT_INT0
{
BYTE temp;
#ifndef _TESTMODE_
  temp = L01_Get_Status();                                        // Read status, get IRQ source & pipe#
  switch(temp & MASK_IRQ_FLAGS)                                   // Get IRQ source
  {
    case RX_DR:                                                   // Data Ready RX FIFO ?
      IRQ_Source = RX_DR;                                         // Flag for RX_DR
    break;

    case TX_DS:                                                   // Data Sent TX FIFO ?
      IRQ_Source = TX_DS;                                         // Flag for TX_DS
    break;

    case MAX_RT:                                                  // Max #of retries ?
      IRQ_Source = MAX_RT;                                        // Flag for MAX_RT
    break;

    default:                                                      // Default, clear IRQ flags
      L01_Clear_IRQ(MASK_IRQ_FLAGS);
    break;
  }

#else                                                             // _TESTMODE_, "production test"
  temp = L01_Clear_IRQ(MASK_IRQ_FLAGS);                           // Read status & clear IRQ flag's
  switch(temp & MASK_IRQ_FLAGS)
  {
    case MAX_RT:                                                  // Max Retries
      Led3 = 0;                                                   // DEBUG Led3 lit
    break;
    case TX_DS:                                                   // TX data sent
      Led4 = 0;                                                   // DEBUG Led4 lit
    break;
    case RX_DR:                                                   // RX data received
      Led4 = 0;                                                   // DEBUG Led4 lit
      L01_Read_RX_Pload(RX_pload);                                // Read data from current pipe
    break;
  }
#endif
}

//****************************************
//
// Timer2 Compare Match interrupt
//
// 
// 
//
// 
//
//****************************************
void T2_Comp_Match(void) interrupt T2_INTERRUPT
{

  if(Test_Mode)                                                   // Timer for Test_Mode?
  {
    if(Sweep_Mode)                                                // Sweep mode in Test Mode application
    {
      if(++Sweep_Time_Ctr == 20)                                  // Timeout Sweep counter, 20ms
      {
        Sweep_Time_Ctr = 0;                                       // Clear 'Sweep_Time_Ctr' on every match
        Led1 = ~Led1;                                             // _DEBUG_, every channel inc..

        CE_Pin(CE_LOW);                                           // Change channel, CE must be low
        L01_Set_Channel(next_ch);                                 // set next channel
        CE_Pin(CE_HIGH);                                          // Set CE high again

        if (++next_ch > sw_high)                                  // reach high channel
        {
          next_ch = sw_low;                                       // yes; current_ch = low channel
          Led2 = ~Led2;                                           // _DEBUG_, every "wrap"
        }
      }
    }
    else
    {
      if(Sweep_Progress)                                          // Terminate "Sweep_Mode"
      {
        CE_Pin(CE_LOW);                                           // End sweep mode with CE low
        Sweep_Progress = 0;                                       // Reset sweep in progress flag
        L01_Flush_TX();                                           // Flush TX to terminate further interrupts
        Sweep_Time_Ctr = 0;                                       // Clear Sweep_Time_Ctr on every match

#ifndef _TESTMODE_
        Write_Led(LED1, LED_OFF);                                 // Sweep mode terminated, clear Led1
        Write_Led(LED2, LED_OFF);                                 // Sweep mode terminated, clear Led1
#endif
      }
    }
  }
  
  if((Com_Mode == TX_MODE) && Timer_Mode)
  {
    if(Trans_Tmr == ++Trans_Tmr_Ctr)                            // timeout; TX packets interval
    {

      switch(LinkStat)
      {
        case LINK_LOSS:                                         // first attempt after link loss..
          CE_Pin(CE_PULSE);                                     // retransmitt!
        break;

        case LINK_ESTABLISH:                                    // normal operation
          L01_Write_TX_Pload(TX_pload, TX_pload_width);         // write new TX payload,
          CE_Pin(CE_PULSE);                                     // and transmitt packet.
          Trans_Tmr_Ctr = CLEAR;                                // clear Transmitt Timer
        break;

        case LINK_RELOST:                                       // do nothing if RELOST, i.e. channel scanning..

        break;
      }

    }

  }

  if(Com_Mode == RX_MODE)                                         // Timer mode for RX device
  {
    if(Trans_Tmr == ++Trans_Tmr_Ctr)                              // Timeout; Frequency Agility Timer
    {
      Blink_Led(LED3);
      LinkStatus[LINK_STATUS] = LINK_LOSS;                        // Message; LINK LOSS
      LinkStatus[LINK_CHANNEL] = L01_Get_Channel();               // on channel:LINK_CHANNEL
      LinkStat = LINK_LOSS;                                       // Current "Link Status = LINK_LOSS"

      CE_Pin(CE_LOW);                                             // CE low during channel switch
      if(++Table_Ptr == FREQ_TABLE_SIZE)                          // Increment freq_table pointer
      {
        Blink_Led(LED4);
        Table_Ptr = CLEAR;                                        // Wrap freq_table pointer
      }
      L01_Flush_RX();                                             // FAP timeout, Flush RX FIFO.
      L01_Set_Channel(Freq_table[Table_Ptr]);                     // Change channel
      CE_Pin(CE_HIGH);                                            // then set CE high again
      Agil_Timeout = TIMEOUT;                                     // default, agil_timeout, only reset by RX_DR
      Trans_Tmr_Ctr = CLEAR;                                      // Clear counter
    }

    Update_Link_Status();                                         // Link_Status used for "Events:" RX mode
    
    if(LinkStat == LINK_LOSS)
    {
      Link_Loss_Delay++;
    }
    else
    {
      Link_Loss_Delay = 0;
    }

  }
  TF2H = 0;                                                       // Reset T2 interrupt flag
}


/*****************************************
*
* Timer0 Overflow interrupt
*
* Used for CE pulse signal
* Timeout:12µs
*
* 
*
*****************************************/
void T0_Overflow(void) interrupt T0_INTERRUPT
{
  TR0 = 0;                                                        // Stop T0
  CE = 0;                                                         // Reset CE signal
}

/*****************************************
*
* Timer1 Overflow interrupt
*
* Used for Led blink..
* Timeout:248µs
*
* 
*
*****************************************/
void T1_Overflow(void) interrupt T1_INTERRUPT
{
  if(Led_Blink1 == 1)     // LED1
  {
    Led1 = 1;
    Led_Blink1--;
  }  
  if(Led_Blink1 == 2)
    Led_Blink1--;

  if(Led_Blink2 == 1)     // LED2
  {
    Led2 = 1;
    Led_Blink2--;
  }  
  if(Led_Blink2 == 2)
    Led_Blink2--;

  if(Led_Blink3 == 1)     // LED3
  {
    Led3 = 1;
    Led_Blink3--;
  }  
  if(Led_Blink3 == 2)
    Led_Blink3--;

  if(Led_Blink4 == 1)     // LED4
  {
    Led4 = 1;
    Led_Blink4--;
  }  
  if(Led_Blink4 == 2)
    Led_Blink4--;

}

/*****************************************
*
* USB Interrupt Service Rutine
*
* 
* 
*
* 
*
*****************************************/
void USB_Interrupt(void) interrupt USB_INTERRUPT
{
  BYTE INTVAL = Get_Interrupt_Source();
  if (INTVAL & RX_COMPLETE)
  {
    Block_Read(RX_buffer,USB_RX_SIZE);
    USB_Rx_Comp = 1;
  }

  if (INTVAL & DEV_SUSPEND)
  {
//    Suspend_Device();                                           // ..suspend has been disabled..
  }

  if (INTVAL & DEV_CONFIGURED)
  {
    Port_Init();
  }
}

