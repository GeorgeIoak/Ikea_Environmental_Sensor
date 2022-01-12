/** @file F320_FlashPrimitives.h
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
 * $Revision: 2 $
 *
 */
#ifndef F320_FLASHPRIMITIVES_H
#define F320_FLASHPRIMITIVES_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------
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


typedef unsigned long ULONG;
//typedef unsigned int UINT;
typedef unsigned char UCHAR;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512
#endif

#ifndef FLASH_TEMP
#define FLASH_TEMP 0x03a00L
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0x03c00L
#endif

typedef UINT FLADDR;

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
extern void FLASH_ByteWrite (FLADDR addr, char byte);
extern unsigned char FLASH_ByteRead (FLADDR addr);
extern void FLASH_PageErase (FLADDR addr);

#endif // F320_FLASHPRIMITIVES_H