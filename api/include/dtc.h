/**
 * @file dtc.h
 * @brief DTC macros.
 *
 */


#ifndef _OSCC_DTC_H_
#define _OSCC_DTC_H_


/*
 * @brief Set a DTC bit in a DTC bitfield.
 *
 */
#define DTC_SET( bitfield, dtc ) ( (bitfield) |= (1<<(dtc)) )

/*
 * @brief Clear a DTC bit in a DTC bitfield.
 *
 */
#define DTC_CLEAR( bitfield, dtc ) ( (bitfield) &= ~(1<<(dtc)) )

/*
 * @brief Check if a DTC bit in a DTC bitfield is set.
 *
 */
#define DTC_CHECK( bitfield, dtc ) ( (bitfield) & (1<<(dtc)) )


#endif /* _OSCC_DTC_H_ */
