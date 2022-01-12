/** @file low_level.h
 *
 * Register handler. This file contains routines used when interfacing the nRF24Z1 with the slave
 * and master interfaces.
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
 * $Revision: 24 $
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

// Pin definitions and init values
#define P0_DIN    0xff              // Port0; digital I/O
#define P1_DIN    0x7f              // Port1; P1.7 analog input, rest. digital I/O
#define P2_DIN    0xff              // Port2; digital I/O
#define P3_DIN    0x01              // Port3; P3.0 digital I/O

#define P0_DOUT   0x1D              // Port0, nRF24L01 interface; P0.5=In, P0.4=Out, P0.3=Out, P0.2=Out, P0.1=In, P0.0=Out
#define P1_DOUT   0x0f              // Port1, VDD_RF & LED1..4
#define P2_DOUT   0x00              // Port2, USB-ID & B1..3, digital inputs, open-drain, weak pull-ups
#define P3_DOUT   0x00

#define P0_INIT   0x2A              // Port0 init; P0.5=HiZ, P0.4=L, P0.3=H, P0.2=L, P0.1=HiZ, P0.0=L
#define P1_INIT   0x0f              // Port1 init; P1.7=Analog, P1[6..4] = L, P1[3..0] = H
#define P2_INIT   0xff              // Port2 init; P2[7..0] = HiZ
#define P3_INIT   0x01              // Port3 init; P3.0 = HiZ

// Crossbar configuration
#define XBAR0     0x02              // SPI Enabled
#define XBAR1     0x40              // Crossbar enabled

// LED definitions
#define LED1      0x00
#define LED2      0x01
#define LED3      0x02
#define LED4      0x03

#define LED_OFF   0x00
#define LED_ON    0x01

// CE_Action definitions
#define CE_LOW    0x00
#define CE_HIGH   0x01
#define CE_PULSE  0x02

sbit Led1 =       P1^0;
sbit Led2 =       P1^1;
sbit Led3 =       P1^2;
sbit Led4 =       P1^3;

// Button's definitions
sbit SW1  =       P2^1;
sbit SW2  =       P2^2;
sbit SW3  =       P2^3;

// Rotary SW readout
#define USB_ID    ((~(P2 & 0xf0))>>4)
#define USB_SW    (P2 >> 4)

// Define interface to nRF24L01
#ifndef _SPI_PIN_DEF_
#define _SPI_PIN_DEF_
// Define SPI pins
sbit SCK  =       P0^0;             // Master Out, Slave In pin (output)
sbit MISO =       P0^1;             // Master In, Slave Out pin (input)
sbit MOSI =       P0^2;             // Serial Clock pin, (output)
sbit CSN  =       P0^3;             // Slave Select pin, (output to CSN, nRF24L01)

// Define CE & IRQ pins
sbit CE   =       P0^4;             // Chip Enable pin signal (output)
sbit IRQ  =       P0^5;             // Interrupt signal, from nRF24L01 (input)
#endif

// Interrupt vectors numbers
#define EXT_INT0            0x00
#define T0_INTERRUPT        0x01
#define T1_INTERRUPT        0x03
#define T2_INTERRUPT        0x05
#define USB_INTERRUPT       0x10

// Parameters used for testmodes: 'Test_RX' and 'Test_TX'
// Preprosessor directive to include debug functions
// #define _TESTMODE_

#define FW_VERSION          0x0200
#define VID_NORDIC          0x1915
#define PID_NRF24L01_EC     0x1000
#define VID_SILABS          0x10C4
#define PID_SILABS          0xEA61

// LL_API.h prototypes
void Port_Init(void);
void WD_Reset(void);
void Reset_L01(void);
void Start_Sweep_Timer(void);
void Stop_Sweep_Timer(void);
void Init_T0(void);
void Start_T1(void);
void Init_T2(void);
void CPU_Init(void);
void Write_Led(BYTE led, BYTE state);
void Toggle_Led(BYTE led);
void Start_Communication(BYTE Com_Mode);

void CE_High(void);
void CE_Low(void);
void Pulse_CE(void);
void CE_Pin(BYTE action);
void CSN_Pin(BYTE state);
void SCK_Pin(BYTE state);
void MOSI_Pin(BYTE state);
BYTE MISO_Pin(void);
BYTE SPI_HW(BYTE byte);
void SPI_Init(BYTE Mode);
void Blink_Led(BYTE led);
void Update_Link_Status(void);
BYTE Send_Packet_Button(void);
void Send_Packet_Retry(void);
void Check_Button_TX(void);
void Start_T2(void);
void Stop_T2(void);
