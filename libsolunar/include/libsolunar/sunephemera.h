/*============================================================================
  
  libsolunar 
  
  sunephemera.h

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

BEGIN_DECLS

/** Get the sun's right ascension and declination at the specified
 * time. RA is in hours, dec in degrees. */
extern void    sunephemera_get_ra_and_dec (time_t t, double *ra, double *dec);

/* Get the sine of the sun's angle above the horizon, at a specific
 * time and location. A negative result indicates that the sun is
 * below the horizon for the observer. */
extern double  sunephemera_get_sin_altitude (double latitude, 
                  double longitude, time_t t);


END_DECLS

