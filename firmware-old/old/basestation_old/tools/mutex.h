/*
 * mutex.h
 *
 *  Created on: 29.04.2015
 *      Author: christoph
 *
 *      taken from the ARM-manual:
 *      http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHEJCHB.html
 */

#ifndef MUTEX_H_
#define MUTEX_H_

void get_lock(volatile unsigned int *Lock_Variable);

void free_lock(volatile unsigned int *Lock_Variable);


#endif /* MUTEX_H_ */
