/**
 * @file magic.h
 * @brief Definitions of the magic bytes identifying messages as from OSCC.
 *
 */


#ifndef _OSCC_MAGIC_H_
#define _OSCC_MAGIC_H_


/*
 * @brief First magic byte used in commands and reports to distinguish CAN
 *        frame as coming from OSCC (and not OBD).
 *
 */
#define OSCC_MAGIC_BYTE_0 ( 0x05 )

/*
 * @brief Second magic byte used in commands and reports to distinguish CAN
 *        frame as coming from OSCC (and not OBD).
 *
 */
#define OSCC_MAGIC_BYTE_1 ( 0xCC )


#endif /* _OSCC_MAGIC_H_ */
