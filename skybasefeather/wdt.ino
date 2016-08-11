#include "wdt.h"

void WDT_init() {
  // disable watchdog for sure
  WDT->CTRL.bit.ENABLE = 0;

  // configure clock for WDT
  
  GCLK->GENDIV.reg  = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(2);
  //GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_GENEN;
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_WDT_Val) |
                      GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_CLKEN;

  // sync clock
  while (GCLK->STATUS.bit.SYNCBUSY) {}

  // configure watchdog (about 3 seconds with clock from above)
  WDT->CONFIG.bit.PER = 0xB;  // 1024 clock cycles

}
void WDT_enable() {
  // enable watchdog
  WDT->CTRL.bit.ENABLE = 1;
  while (WDT->STATUS.bit.SYNCBUSY) {}
  WDT_clear();
}
void WDT_clear() {
  WDT->CLEAR.bit.CLEAR = 0xA5;
}
