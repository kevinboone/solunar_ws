/*============================================================================
  
  libsolunar 
  
  astroutil.h

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

BEGIN_DECLS

/** Get the difference in hours between the specified longitude and the
     Greenwich meridian -- 360/24 degrees per hour */
extern double astroutil_get_hours_from_meridian (double longitude);

/** Get  the local mean siderial time corresponding to the specified
 * time and (observer) longitude. See:
https://en.wikipedia.org/wiki/Sidereal_time. As usual, a positive longitude
is to the east of the Greenwich meridian.
*/
double astroutil_lmst (time_t t, double longitude);

/* Get the sine altitude of a body with specified RA and declination,
 * as seen by an observer at the specified latitude and longitude. */
double astroutil_ra_dec_to_sin_altitude (time_t t, double latitude, 
        double longitude, double ra, double dec);

END_DECLS
