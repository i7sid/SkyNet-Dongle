/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2016
 * (c) NXP Semiconductors 2013-2016
 * Linker script memory definitions
 * Created from LinkMemoryTemplate
 * By LPCXpresso v7.5.0 [Build 254] [2014-10-31]  on Tue Apr 05 13:27:49 CEST 2016)
*/

MEMORY
{
  /* Define each memory region */
  MFlash512 (rx) : ORIGIN = 0x10000, LENGTH = 0x70000 /* 448K bytes */
  RamLoc32 (rwx) : ORIGIN = 0x10000100, LENGTH = 0x7ef0 /* 32496 bytes */
  RamBootShared (rwx) : ORIGIN = 0x10007ff0, LENGTH = 0x10 /* 16 bytes */
  RamAHB32 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x8000 /* 32K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlash512 = 0x10000 + 0x70000;
  __top_RamLoc32 = 0x10000100 + 0x7ef0;
  __top_RamBootShared = 0x10007ff0 + 0x10;
  __top_RamAHB32 = 0x2007c000 + 0x8000;



