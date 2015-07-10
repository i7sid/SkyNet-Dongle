/** @file
 *
 *  @date 09.07.2015
 *  @author Christoph Kohl
 *	@brief Configurable Values
 */

#include "config.h"

bool skipcompasscal = true;
bool skipgpsfix = false;
int magneticdeclination = 258; // Erlangen, Germany 49°35' 11°0', Magnetic Declination 2°35'
char * deviceID = "01";
unsigned int bufferedtime = 900;
unsigned int sendtime = 900;
