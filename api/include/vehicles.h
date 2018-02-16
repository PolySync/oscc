/**
 * @file vehicles.h
 * @brief List of vehicle headers.
 *
 */


#ifndef _OSCC_VEHICLES_H_
#define _OSCC_VEHICLES_H_


#if defined(KIA_SOUL)
#include "vehicles/kia_soul_petrol.h"
#elif defined(KIA_SOUL_EV)
#include "vehicles/kia_soul_ev.h"
#elif defined(KIA_NIRO)
#include "vehicles/kia_niro.h"
#endif

#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))


#endif /* _OSCC_VEHICLES_H_ */
