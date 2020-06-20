/*============================================================================
  
  libsolunar 
  
  moonephemera.h

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

BEGIN_DECLS

/** Get the moon's right ascension and declination at the specified
 * time. RA is in hours, dec in degrees. */
extern void  moonephemera_get_ra_and_dec (time_t t, double *ra, double *dec);

extern void moonephemera_get_moon_state (double latitude, double longitude, 
               time_t time, const char **phase_name, 
               double *phase, double *age, double *distance);

/** Get the moon phase in English for the specified phase value, 
 * which lies between 0 (new) and 1 (new) with full at 0.5 */
const char *moonephemera_get_phase_name (double phase);

/* Get the sine of the moon's angle above the horizon, at a specific
 * time and location. A negative result indicates that the sun is
 * below the horizon for the observer. */
extern double moonephemera_get_sin_altitude (double latitude, 
                  double longitude, time_t t);

END_DECLS


