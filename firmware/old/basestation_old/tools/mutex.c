/*
 * mutex.c
 *
 *  Created on: 29.04.2015
 *      Author: christoph
 */

#include "../../basestation_old/tools/mutex.h"

#include "cmsis.h"

void get_lock(volatile unsigned int *Lock_Variable)
{ // Note: __LDREXW and __STREXW are CMSIS functions
	int status = 0;
	do {
		while (__LDREXW(Lock_Variable) != 0); // Wait until
		// Lock_Variable is free
		status = __STREXW(1, Lock_Variable); // Try to set
		// Lock_Variable
	} while (status!=0); //retry until lock successfully
	__DMB();		// Do not start any other memory access
	// until memory barrier is completed
	return;
}

void free_lock(volatile unsigned int *Lock_Variable)
{ // Note: __LDREXW and __STREXW are CMSIS functions
	__DMB(); // Ensure memory operations completed before
	// releasing lock
	Lock_Variable = 0;
	return;
}
