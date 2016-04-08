/**
 * @file
 *
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-02-02
 */

#ifndef MAC_CONFIG_H
#define MAC_CONFIG_H

/*
#define MAC_CONST_aMinMPDUOverhead          (9)
#define MAC_CONST_aUnitBackoffPeriod        (20)
#define MAC_CONST_aMaxMPDUUnsecuredOverhead (25)
#define MAC_CONST_aMaxSIFSFrameSize         (18)
//#define MAC_CONST_aMaxMACPayloadSize        (  )
#define MAC_CONST_aBaseSlotDuration         (60)
*/

//#define MAC_CONST_SYMBOL_LENGTH             (10) // TODO: correct value in ms?
#define MAC_CONST_SYMBOL_LENGTH             (1) // TODO: correct value in ms?

#define MAC_CONST_A_UNIT_BACKOFF_PERIOD     (20) // in "symbols"
#define MAC_CONST_MAX_BE                    (8)
#define MAC_CONST_MIN_BE                    (3)
#define MAC_CONST_MAX_CSMA_BACKOFFS         (4)


#endif /* !MAC_CONFIG_H */
