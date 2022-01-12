 /** @file main.c
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
 * $Date: 3.03.06 10:52 $
 * $Revision: 60 $
 *
 */
//  Global defines & includes
#include <cygnal\c8051F320.h>
#include "include\defines.h"
#include "include\usb.h"
#include "include\fwupgrade.h"
#include "include\F320_FlashPrimitives.h"
#include "include\nRF_API.h"
#include "include\ADC.h"
#include "include\TestApp.h"
#include "include\LL_API.h"
#include "include\Protocol_API.h"

code const UINT FW  = FW_VERSION;                                     // Firmware version, const..

BYTE xdata RX_buffer[USB_RX_SIZE];                                    // Last packet received from host

BYTE xdata TX_buffer[USB_TX_SIZE];                                    // Next packet to sent to host

BYTE xdata TX_pload[TX_PLOAD_MAX_WIDTH];                              // TX payload buffer
BYTE xdata RX_pload[RX_PLOAD_MAX_WIDTH];                              // RX payload buffer

BYTE xdata Table_Ptr;                                                 // Pointer for Freq_table lookup
BYTE xdata Freq_table[FREQ_TABLE_SIZE] = {2,27,52,7,32,57,12,37,
                                         62,17,42,67,22,47,72,77};    // Freq Agility table, predefined

BYTE xdata Trans_Ctrl[TRANS_PARAMS];                                  // Transmission Control bytes
BYTE xdata Freq_agil[NUM_OF_PIPES];                                   // Freq Agility On/Off for pipe.n
BYTE xdata TX_Addr[TX_ADDR_WIDTH];                                    // TX address buffer
BYTE xdata RX_pload_length[NUM_OF_PIPES];                             // holds #of bytes for pipe 0..5
BYTE xdata TX_pload_width;                                            // TX payload width variable
BYTE xdata Freq_Agil_Status;                                          // Variable that holds status of frequency agility
BYTE xdata Freq_Agil_Pipe;

BYTE xdata Button_Mode = 0;
BYTE xdata Button;                                                    // Variable that holds button index for communication
BYTE xdata SW1_Released;

//Status bytes
BYTE USB_Rx_Comp = 0;                                                 // Indicates new USB frame received
BYTE SPI_Mode = 0;                                                    // Select between HW and SW SPI
BYTE Sweep_Mode = 0;                                                  // Sweep mode on/off
BYTE Com_Mode = 2;                                                    // Communication mode on/off
BYTE IRQ_Source = 0x00;                                               // Indicates IRQ source from nRF24L01
BYTE Sweep_Progress;
BYTE Test_Mode;
BYTE Agil_Timeout;
BYTE LinkStatus[2];                                                   // Link status array, status & channel
BYTE LinkStat,LastStat;
BYTE TX_Update;
BYTE RX_Timeout;
UINT Link_Loss_Delay;                                                 // Link Loss dalay variable
BYTE Link_Loss_Status;

BYTE RX_pipe;                                                         // store RX_FIFO pipe number
BYTE byte_ctr;

// Variables used for 'Sweep_Mode'
BYTE sw_low,sw_high,next_ch;
BYTE Sweep_Time_Ctr = 0;

BYTE Trans_Tmr,Trans_Tmr_Ctr = 0;                                     // Variables for trans timer comm mode
BYTE Timer_Mode;                                                      // Variable for timer mode
BYTE Try_Ctr;                                                         // Try Counter variable
BYTE Pipe_Length;                                                     // Current RX pipe length variable
UINT CPU_temp, RF_Vdd;
BYTE Led_Blink1,Led_Blink2,Led_Blink3,Led_Blink4;
BYTE T2_State;

//--------------------------------------------------------------------
// Main Routine
//--------------------------------------------------------------------
void main(void)
{
  CPU_Init();

  while (TRUE)
  {
#ifdef _TESTMODE_
    while (!USB_Rx_Comp)                                              // wait for USB command
    {
      if(!SW1) Test_TX();                                             // Goto TX mode
      if(!SW2) Test_RX();                                             // Goto RX mode
      if(USB_SW != 0x0f)                                              // Enable SW3 = CE if USB_ID <> 0.
      {
        if(!SW3)
        {
          CE_Pin(CE_HIGH);                                            // Hold CE high while SW3 is pressed.
        }
        else 
        {
          CE_Pin(CE_LOW);                                             // Hold CE low as long as SW3 is released.
        }
      }
    }
#else
    while(!USB_Rx_Comp)                                               // wait for USB command, and prosess other operations
    {
      if(IRQ_Source)                                                  // wait for nRF24L01 interrupt
      {
      EX0 = 0;                                                        // Disable External interrupt during prosessing
      if(T2_State)
        {
          ET2 = 0;                                                    // Disable T2 interrupt
          Stop_T2();                                                  // and stop timer2
        }
        switch(IRQ_Source)
        {
          case RX_DR:                                                 // RX Data received interrupt                                          
            do
            {
              RX_pipe = L01_Read_RX_Pload(RX_pload) >> 8;             // Read current payload
              if(Freq_Agil_Pipe == RX_pipe)                           // Packet received for "expected" pipe?
              {
                Blink_Led(LED1);                                      // _DEBUG_LED_ Led1 BLINK
                LinkStatus[LINK_STATUS] = LINK_ESTABLISH;             // Indicate "Link Loss"
                LinkStatus[LINK_CHANNEL] = L01_Get_Channel();         // Insert current channel
              
                LinkStat = LINK_ESTABLISH;                            // New LinkStat
                Agil_Timeout  = CLEARED;                              // Packet received, i.e. ok
                Trans_Tmr_Ctr = CLEAR;                                // Reset frequency agility timer
              }
              else
                Blink_Led(LED2);                                      // _DEBUG_LED_ Led2 BLINK if data of other pipe
            }
            while(!(L01_Get_FIFO() & RX_EMPTY));                      // ..until FIFO empty

            L01_Clear_IRQ(MASK_RX_DR_FLAG);                           // clear RX_DR flag
          break;
          
          case TX_DS:                                                 // TX Data sent interrupt
            Blink_Led(LED1);                                          // _DEBUG_LED_ Led1 BLINK
            LinkStat = LINK_ESTABLISH;                                // set LinkStat = LINK_ESTABLISH
            Trans_Tmr_Ctr = CLEAR;                                    // reset Transmitt Timer..

            L01_Clear_IRQ(MASK_TX_DS_FLAG);                           // Clear TX_DS flag
          break;

          case MAX_RT:
          {
            Blink_Led(LED3);                                          // _DEBUG_LED_ Led3 BLINK

            if(Freq_Agil_Status)                                      // running in freq_agil mode?
            {
              if(++Table_Ptr == FREQ_TABLE_SIZE)                      // Freq_table wrapping?
              {
                Blink_Led(LED4);                                      // _DEBUG_LED_ Led4 BLINK
                Table_Ptr = CLEAR;                                    // Reset Table_Ptr to first channel,
                if(Try_Ctr++ == 3)
                {
                  LinkStat = STOP;
                }
              }
              L01_Set_Channel(Freq_table[Table_Ptr]);                 // select new channel
            }
            
            Trans_Tmr_Ctr = CLEAR;                                    // reset Transmitt Timer..
            switch(LinkStat)
            {
              case LINK_ESTABLISH:                                    // max retransmitt, but had comm on last channel
                L01_Clear_IRQ(MASK_MAX_RT_FLAG);                      // clear MAX_RT flag (nRF24L01)
              
                LinkStat = LINK_LOSS;                                 // change LinkStat state to LINK_LOSS, wait for new timeout
              break;

              case LINK_LOSS:                                         // still not connected, run channel scan..
                L01_Clear_IRQ(MASK_MAX_RT_FLAG);                      // clear MAX_RT flag (nRF24L01)
                CE_Pin(CE_PULSE);                                     // retransmitt packet
              
                LinkStat = LINK_RELOST;                               // change LinkStat state to LINK_RELOST
              break;
            
              case LINK_RELOST:
                L01_Clear_IRQ(MASK_MAX_RT_FLAG);                      // clear MAX_RT flag (nRF24L01)
                CE_Pin(CE_PULSE);                                     // retransmitt packet
              
                LinkStat = LINK_RELOST;
              break;

              case STOP:
                L01_Clear_IRQ(MASK_MAX_RT_FLAG);                      // clear MAX_RT flag
                LinkStat = LINK_ESTABLISH;                            // stop retransmission
              break;
            }
          }
          break;

          default:                                                    // ..error handler
          
          break;
        }
        IRQ_Source = CLEAR;                                           // Clear IRQ flag

        if(T2_State)
          {
            Start_T2();
            ET2 = 1;                                                  // Enable T2 interrupt only if T2 in use          
          }

        EX0 = 1;                                                      // Enable external interrupt again..
      }
      
      Check_Button_TX();

    }


#endif
    Sweep_Mode = CLEAR;                                               // Terminate Sweep_Mode
    USB_Rx_Comp = CLEAR;
    switch(RX_buffer[CMD])                                            // USB command decoder
    {
      case CSN_STATE:                                                 // Set/Reset CSN(NSS) pin
        CSN_Pin(RX_buffer[ARG]);
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case SPI_COMMAND:                                               // Writes a SPI byte
        TX_buffer[0] = SPI_RW(RX_buffer[ARG]);
        Block_Write(TX_buffer,1);
      break;

      case CE_BIT:                                                    // Set/Reset CE pin
        CE_Pin(RX_buffer[ARG]);
        
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case FW_VER:                                                    // Return current FirmWare version
        TX_buffer[0] = (BYTE)(FW>>8);                                 // Get major FW_Ver
        TX_buffer[1] = (FW & 0xff);                                   // Get minor FW_Ver
        
        Block_Write(TX_buffer, 2);
      break;

      case READ_USB_ID:                                               // Return current USB_ID switch setting
        TX_buffer[0] = USB_ID;                                        // macro; defines.h
        Block_Write(TX_buffer,1);
      break;

      case WRITE_LEDS:                                                // Set/Reset LED...
        Write_Led(RX_buffer[ARG], RX_buffer[VALUE]);
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case WR_FLASH_BLOCK:                                            // Writes 32 bytes of flash data to block 'n'
        WritePageBlock(RX_buffer[ARG], &RX_buffer[VALUE]);
        TX_buffer[0] = 0x00;
        Block_Write(TX_buffer,1);
      break;

      case RD_FLASH_BLOCK:                                            // Reads and return 32 bytes of flash block 'n'
        ReadPageBlock(RX_buffer[ARG], &TX_buffer[0]);
        Block_Write(TX_buffer, SZ_BLOCK);
      break;

      case WR_FLASH_PAGE:                                             // Writes the flash page n*256 buffer to flash
        if (RX_buffer[ARG] == 0)
          EA = 0;
        WriteFlashPage((unsigned)RX_buffer[ARG]);
        if (RX_buffer[ARG] == 0)
          EA = 1;

        TX_buffer[0] = 0x00;
        Block_Write(TX_buffer,1);
      break;

      case RD_FLASH_PAGE:                                             // Read the n*256 flash page to flash buffer
        ReadFlashPage((unsigned)RX_buffer[ARG]);
        TX_buffer[0] = 0x00;
        Block_Write(TX_buffer,1);
      break;

      case WD_RESET:                                                  // Use WatchDog reset to reset F320
        WD_Reset();
      break;

      case READ_LOCK_BYTE:                                            // Return Lock Byte
        TX_buffer[0] = FLASH_ByteRead(0x3dff);
        Block_Write(TX_buffer, 1);
      break;

      case SPI_SELECT:                                                // Select SPI Mode
        SPI_Init(RX_buffer[ARG]);
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case ST_FR_SWEEP_TX:                                            // Start Frequency Sweep TX[sw_low..sw_high]
        sw_high = RX_buffer[1];
        sw_low  = RX_buffer[2];
        next_ch = sw_low;                                             // Init sweep low channel
        Sweep_Mode = 1;                                               // Sweep_Mode on.
        Sweep_Progress = 1;                                           // Indicates sweep in progress
        ET2 = 1;                                                      // Enable Timer2
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case ST_FR_SWEEP_RX:                                            // Start Frequency Sweep RX[sw_low..sw_high]
        sw_high = RX_buffer[1];
        sw_low  = RX_buffer[2];
        next_ch = sw_low;                                             // init sweep low channel
        Sweep_Mode = 1;                                               // Sweep_Mode on.
        Sweep_Progress = 1;                                           // Indicates sweep in progress
        ET2 = 1;                                                      // Enable Timer2
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case STOP_SWEEP:                                                // Stop current frequency sweep
        Sweep_Mode = 0;                                               // Terminate Sweep_Mode

        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case WR_TRANS_CTRL:                                             // Set TX control parameters
        Trans_Ctrl[0] = RX_buffer[ARG];
        Trans_Ctrl[1] = RX_buffer[VALUE];

        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case RD_TRANS_CTRL:                                             // Return TX control parameters
        TX_buffer[0] = Trans_Ctrl[0];
        TX_buffer[1] = Trans_Ctrl[1];
        Block_Write(TX_buffer,2);
      break;

      case WR_FREQ_AGIL:                                              // Set freq. agility parameters to pipe.n variabel
        Freq_agil[RX_buffer[ARG]] = RX_buffer[VALUE];

        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case RD_FREQ_AGIL:                                              // Return freq.agility parameters for pipe.n
        TX_buffer[0] = Freq_agil[RX_buffer[ARG]];
        Block_Write(TX_buffer,1);
      break;

      case WR_CH_TABLE:                                               // Write freq_table contents
        for(byte_ctr=0;byte_ctr<FREQ_TABLE_SIZE;byte_ctr++)
          Freq_table[byte_ctr] = RX_buffer[ARG + byte_ctr];

        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case RD_CH_TABLE:                                               // Read freq_table data
        for(byte_ctr=0;byte_ctr<FREQ_TABLE_SIZE;byte_ctr++)
          TX_buffer[byte_ctr] = Freq_table[byte_ctr];

        Block_Write(TX_buffer,FREQ_TABLE_SIZE);
      break;

      case WR_TX_PAYLOAD:                                             // Write TX payload data
        for(byte_ctr=0;byte_ctr<RX_buffer[ARG];byte_ctr++)
          TX_pload[byte_ctr] = RX_buffer[(byte_ctr+2)];               // Load TX payload into buffer
        TX_pload_width = RX_buffer[ARG];                              // Write TX_pload witdh

        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case READ_TX_PLOAD:                                             // Host read TX payload
        TX_buffer[0] = TX_pload_width;
        for(byte_ctr=0;byte_ctr < TX_pload_width;byte_ctr++)
          TX_buffer[byte_ctr+1] = TX_pload[byte_ctr];                 // Load TX_buffer with TX payload data

        Block_Write(TX_buffer, (TX_pload_width + 1));
      break;
      
      case WR_RX_PLOAD_LENGTH:                                        // Write RX payload length for pipe.n
        RX_pload_length[RX_buffer[ARG]] = RX_buffer[VALUE];
        
        TX_buffer[0] = 0;
        Block_Write(TX_buffer, 1);
      break;

      case RD_RX_PLOAD_LENGTH:                                        // Read RX payload length for pipe.n
        TX_buffer[0] = RX_pload_length[RX_buffer[ARG]];

        Block_Write(TX_buffer, 1);
      break;

      case START_COM_MODE:                                            // Start communication mode
        Com_Mode = RX_buffer[ARG];
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);

        Start_Communication(Com_Mode);
      break;

      case READ_TEMP:                                                 // Read C8051F320 temperature
        CPU_temp = Read_Temperature();
        TX_buffer[0] = (CPU_temp >> 8);
        TX_buffer[1] = (CPU_temp & 0xff);
        Block_Write(TX_buffer,2);
      break;

      case READ_VOLT:                                                 // Read RF_Vdd voltage
        RF_Vdd = Read_RF_Vdd();
        TX_buffer[0] = (RF_Vdd >> 8);
        TX_buffer[1] = (RF_Vdd & 0xff);
        Block_Write(TX_buffer,2);
      break;

      case READ_RX_DATA:                                              // Read RX data
      break;

      case ENTER_TEST_MODE:                                           // Enter testmode
        Enter_Testmode(RX_buffer[ARG]);
        TX_buffer[0] = 0;
        Block_Write(TX_buffer,1);
      break;

      case WR_FREQ_AGILITY:                                           // Enable/ disable freq.agility
        Freq_Agil_Status = RX_buffer[ARG];

        TX_buffer[0] = 0;
        Block_Write(TX_buffer, 1);
      break;

      case RD_FREQ_AGILITY:                                           // Read freq.agility status (enabled or disabled)
        TX_buffer[0] = Freq_Agil_Status;
        Block_Write(TX_buffer, 1);
      break;

      case UPDATE_DEVICE:                                             // Command that indicates a new device update
//        Reset_L01();                                                  // Reset L01 before new update, NOT implemented
        TX_buffer[0] = 0;
        Block_Write(TX_buffer, 1);
      break;

      case STOP_COMM_MODE:                                            // Command received when "Stop Comm.. Mode" button pressed..
        IRQ_Source = CLEAR;
        ET2 = 0;                                                      // Disable T2 interrupt
        EX0 = 0;                                                      // Disable extern interrupt0, nRF24L01_IRQ
        CE_Pin(CE_LOW);                                               // CE low
        Led1 = Led2 = Led3 = Led4 = 1;                                // Clear all status Led's
        Com_Mode = 2;                                                 // Set Com_Mode = 2, idle
        
        TX_buffer[0] = 0;
        Block_Write(TX_buffer, 1);
      break;

      case RD_COMM_MODE_STAT:                                         // Return communication mode status
        TX_buffer[0] = Com_Mode;
        Block_Write(TX_buffer, 1);
      break;

      case RD_LINK_STATUS:                                            // Return Link Status to host
        TX_buffer[0] = LinkStatus[0];
        TX_buffer[1] = LinkStatus[1];
        LinkStatus[0] = LinkStatus[1] = 0;                            // Clear status after status read
        Block_Write(TX_buffer, 2);
      break;

      default:                                                        // Default switch() handler
      break;
    }

  }

}


/*********************************************
*
* Start_Communication
*
* Starts either RX or TX mode
*
*********************************************/

void Start_Communication(BYTE Com_Mode)
{
BYTE pipe, fr_ag_ena=0;
  CE_Pin(CE_LOW);                                                     // CE low before new transmission
  EX0 = 0;                                                            // Disble interrupt0 (nFR24L01 IRQ)
  ET2 = 0;                                                            // Disable timer2 interrupt
  T2_State = 0;
  Stop_T2();
  Try_Ctr = 0;                                                        // Reset Try Counter
  Table_Ptr = 0;                                                      // Reset freq_table pointer
  Link_Loss_Delay = 0;                                                // Reset Link_Loss_Delay.
  Led1=Led2=Led3=Led4=1;                                              // Clear status LED's _DEBUG_?

  L01_Flush_RX();                                                     // Init new device, so FLUSH RX & TX FIFO's
  L01_Flush_TX();
  L01_Clear_IRQ(MASK_IRQ_FLAGS);                                      // Clear interrupts
  IRQ_Source = CLEAR;
  LastStat = LinkStat = LINK_LOSS;                                    // Default LinkStat = LINK_LOSS.  

  if(Com_Mode == TX_MODE)                                             // TX MODE
  {
    LastStat = LinkStat = LINK_ESTABLISH;
    if(Button_Mode = (Trans_Ctrl[TRANS_SOURCE] == BUTTON))            // Button[0..2] selected for communication?
    {
      Timer_Mode = 0;                                                 // Timer mode comm disabled
      LinkStatus[LINK_STATUS] = LINK_ESTABLISH;
    }
    else                                                              // ..else timer selected for communication
    {            
      Button_Mode = 0;                                                // Timer mode enabled, i.e. button disabled
      if(Freq_Agil_Status)                                            // use freq table ONLY if in FAP mode
      {
        L01_Set_Channel(Freq_table[Table_Ptr]);                                   
      }
      Timer_Mode = 1;                                                 // Timer mode comm activated
      Trans_Tmr = Trans_Ctrl[TRANS_VALUE];                            // Init trans timer value
      Trans_Tmr_Ctr = CLEAR;                                          // and clear Transmission counter
      Start_T2();                                                     // Start timer2
      ET2 = 1;                                                        // Enable Timer2 interrupt
      T2_State = 1;                                                   // Indicate T2 running mode
    }
    
  }
  else                                                                // ..else RX mode
  {
    fr_ag_ena = 0;                                                    // init, freq_agil disabled

    for(pipe=0;pipe<6;pipe++)
      L01_WR_RX_PW_n(pipe,RX_pload_length[pipe]);                     // load pipe0..5 with payload width

    if(Freq_Agil_Status)
    {
      L01_Set_Channel(Freq_table[Table_Ptr]);                         // Set first RF channel, !! in Frequency Agility mode !!

      for(pipe=0;pipe<6;pipe++)                                       // Scan all pipes for frequency agility
      {
        if(Freq_agil[pipe])                                           // Check which pipe freq agil is enabled for
        {
          Trans_Tmr = Freq_agil[pipe];                                // Init freq.agil timer
          Freq_Agil_Pipe = pipe;                                      // Frequency agility enabled for pipe:"pipe"
          fr_ag_ena = 1;
        }
      }

      if(fr_ag_ena)                                                   // Init & start timer2 ONLY if freq agility is enabled
      {
        Trans_Tmr_Ctr = CLEAR;                                        // and clear Transmission counter
        Agil_Timeout = TIMEOUT;                                       // Default, no packet received..
        Start_T2();                                                   // Start timer2
        ET2 = 1;                                                      // Enable Timer2 interrupt
        T2_State = 1;
      }

    }

    CE_Pin(CE_HIGH);                                                  // CE high, ready for reception
  }
  EX0 = 1;                                                            // Enable nRF24L01 IRQ
}
