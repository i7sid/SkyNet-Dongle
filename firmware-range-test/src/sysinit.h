#ifndef SYSINIT_H_
#define SYSINIT_H_


/**
 * @brief	Sets up system pin muxing.
 * @author	NXT B.V.
 */
void Board_SetupMuxing(void);


/**
 * @brief	Setup system clocking
 * @author	NXT B.V.
 */
void SystemSetupClocking(void);


/**
 * @brief	Set up and initialize hardware prior to call to main
 * @author	NXT B.V., slightly adapted by Michael Zapf <michael.zapf@fau.de>
 */
void SystemInit(void);


#endif /* SYSINIT_H_ */
