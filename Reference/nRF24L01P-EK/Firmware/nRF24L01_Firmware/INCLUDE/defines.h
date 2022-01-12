/** @file defines.h
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
 * $Revision: 28 $
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

#define FALSE               0x00  // Simple true/false constants
#define TRUE                0x01

#define CLEAR               0x00

#define USB_RX_SIZE         0x22
#define USB_TX_SIZE         0x20

#define FREQ_TABLE_SIZE     0x10
#define TX_PLOAD_MAX_WIDTH  0x20
#define RX_PLOAD_MAX_WIDTH  0x20  // RX payload length = 32 bytes
#define NUM_OF_PIPES        0x06
#define TX_ADDR_WIDTH       0x05
#define TRANS_PARAMS        0x02
#define TRANS_SOURCE        0x00
#define TRANS_VALUE         0x01
#define TIMER               0x00
#define BUTTON              0x01
#define TX_MODE             0x00
#define RX_MODE             0x01
#define MASK_IRQ_FLAGS      0x70
#define MASK_RX_DR_FLAG     0x40
#define MASK_TX_DS_FLAG     0x20
#define MASK_MAX_RT_FLAG    0x10
#define RX_P_NO             0x0e

#define RX_FIFO_EMPTY       0x07
#define RX_EMPTY            0x01

#define PIPE0               0x00
#define PIPE1               0x01
#define PIPE2               0x02
#define PIPE3               0x03
#define PIPE4               0x04
#define PIPE5               0x05

#define LINK_STATUS         0x00
#define LINK_CHANNEL        0x01
#define LINK_NO_MESS        0x00
#define LINK_LOSS           0x01
#define LINK_ESTABLISH      0x02
#define LINK_RELOST         0x03
#define STOP                0x04

#define CLEARED             0x00
#define TIMEOUT             0x01

// FOX low level Commands
//
// Index in USB Command Frame
#define CMD     0x00
#define ARG     0x01
#define VALUE   0x02

// Implemented USB commands
#define CSN_STATE           0x01  // Set state of CSN(SSN) signal
#define SPI_COMMAND         0x02  // Perform a SPI read/write operation
#define CE_BIT              0x03  // Set state of CE signal
#define FW_VER              0x04  // Returns FirmWare version of this system
#define READ_USB_ID         0x05  // Returns the rotary switch value
#define WRITE_LEDS          0x06  // Turn LEDs on/off
#define WR_FLASH_BLOCK      0x07  // Writes 32 bytes of flash data to block 'n'
#define RD_FLASH_BLOCK      0x08  // Reads and return 32 bytes of flash block 'n'
#define WR_FLASH_PAGE       0x09  // Writes the flash page n*256 buffer to flash
#define RD_FLASH_PAGE       0x0a  // Read the n*256 flash page to flash buffer
#define WD_RESET            0x0b  // Perform a WD reset
#define READ_LOCK_BYTE      0x0c  // Returns the F32x device lock byte...
#define SPI_SELECT          0x0d  // Select between HW and SW SPI mode
#define UNUSED              0x0e  // Not used command
#define ST_FR_SWEEP_TX      0x0f  // Start freq.sweep, TX, from channel 'cl' to 'ch' in 20ms intervals
#define ST_FR_SWEEP_RX      0x10  // Start freq.sweep, RX, from channel 'cl' to 'ch' in 20ms intervals
#define STOP_SWEEP          0x11  // Stop either the TX or the RX freq.sweep execution....
#define WR_TRANS_CTRL       0x12  // Select between auto. or manual transmission(TX device)
#define RD_TRANS_CTRL       0x13  // Read Transmit Control state
#define WR_FREQ_AGIL        0x14  // Select Freq. Agility for pipe.n
#define RD_FREQ_AGIL        0x15  // Read freq agility state for pipe.n
#define WR_CH_TABLE         0x16  // Writes the 16 ch's used for freq. agility
#define RD_CH_TABLE         0x17  // Read the 16 ch's used for agility
#define WR_TX_PAYLOAD       0x18  // Writes n bytes of payload data
#define START_COM_MODE      0x19  // Starts the communication mode, (application)
#define READ_TEMP           0x1a  // Returns F320 temperature
#define READ_VOLT           0x1b  // Returns RF_VDD voltage
#define ENTER_TEST_MODE     0x1c  // Ev.board enters production test
#define READ_RX_DATA        0x1d  // Host read RX data on pipe.n
#define READ_TX_PLOAD       0x1e  // Host read current RX payload
#define WR_RX_PLOAD_LENGTH  0x1f  // Host write current RX payload length for pipe.n
#define RD_RX_PLOAD_LENGTH  0x20  // Host read current RX payload length for pipe.n
#define WR_FREQ_AGILITY     0x21  // Command to enable/disable frequency agility
#define RD_FREQ_AGILITY     0x22  // Read previous command's parameter
#define UPDATE_DEVICE       0x23  // This command is sent before the nRF24L01 are beeing updated
#define STOP_COMM_MODE      0x24  // This sommand is sent when "Stop Communication Mode" button is pressed
#define RD_COMM_MODE_STAT   0x25  // Host read current communication mode status (0:TX, 1:RX, 2: IDLE)
#define RD_LINK_STATUS      0x26  // Host read current link status, i.e. message to host, RX device side

