/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2016
 * Generated linker script file for LPC1769
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.1.4 [Build 606] [2016-03-14]  on 05.04.2016 12:10:49
 */

MEMORY
{
  /* Define each memory region */
  MFlash512 (rx) : ORIGIN = 0x10000, LENGTH = 0x70000 /* 448K bytes (alias Flash) */  
  RamLoc32 (rwx) : ORIGIN = 0x10000100, LENGTH = 0x7ef0 /* 32496 bytes (alias RAM) */  
  RamBootShared (rwx) : ORIGIN = 0x10007ff0, LENGTH = 0x10 /* 16 bytes (alias RAM2) */  
  RamAHB32 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x8000 /* 32K bytes (alias RAM3) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash512 = 0x10000  ; /* MFlash512 */  
  __base_Flash = 0x10000 ; /* Flash */  
  __top_MFlash512 = 0x10000 + 0x70000 ; /* 448K bytes */  
  __top_Flash = 0x10000 + 0x70000 ; /* 448K bytes */  
  __base_RamLoc32 = 0x10000100  ; /* RamLoc32 */  
  __base_RAM = 0x10000100 ; /* RAM */  
  __top_RamLoc32 = 0x10000100 + 0x7ef0 ; /* 32496 bytes */  
  __top_RAM = 0x10000100 + 0x7ef0 ; /* 32496 bytes */  
  __base_RamBootShared = 0x10007ff0  ; /* RamBootShared */  
  __base_RAM2 = 0x10007ff0 ; /* RAM2 */  
  __top_RamBootShared = 0x10007ff0 + 0x10 ; /* 16 bytes */  
  __top_RAM2 = 0x10007ff0 + 0x10 ; /* 16 bytes */  
  __base_RamAHB32 = 0x2007c000  ; /* RamAHB32 */  
  __base_RAM3 = 0x2007c000 ; /* RAM3 */  
  __top_RamAHB32 = 0x2007c000 + 0x8000 ; /* 32K bytes */  
  __top_RAM3 = 0x2007c000 + 0x8000 ; /* 32K bytes */  
