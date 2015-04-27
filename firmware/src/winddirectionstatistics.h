/*
 * winddirectionstatistics.h
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 */

#ifndef WINDDIRECTIONSTATISTICS_H_
#define WINDDIRECTIONSTATISTICS_H_

void init_winddir_statistics();

int addvec2(int val1, int val2);

int addvec3(int val1, int val2, int val3);

void input_winddir_statistics(int value);

int ** output_winddir_statistics();

#endif /* WINDDIRECTIONSTATISTICS_H_ */
