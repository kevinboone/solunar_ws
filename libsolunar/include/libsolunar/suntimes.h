/*============================================================================
  
  libsolunar 
  
  suntimes.h

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

/* Define the various zenith values for the sunrise/set calculations.
 * "True" geometrical sunrise would be at a zenith of 90 degrees plus half
 * the angle subtended by the sun. However, most authorities take the
 * true sunrise to be at a zenith a little greater than that, to allow for
 * various effects related to atmospheric effects and the non-uniformity
 * of the sun disk. */
#define SUNTIMES_DEFAULT_ZENITH (90.0 + 50.0/60.0)
#define SUNTIMES_CIVIL_TWILIGHT (90 + 50.0/60.0 + 6)
#define SUNTIMES_NAUTICAL_TWILIGHT (90 + 50.0/60.0 + 12)
#define SUNTIMES_ASTRONOMICAL_TWILIGHT (90 + 50.0/60.0 + 18)

BEGIN_DECLS

/** Get a very approximate sunrise time, relative to midnight UTC at the
 * specified location on the specified day of year. Sunrise is taken to be 6AM
 * UTC at the meridian on that day. There's unlike to be a good reason to call
 * this function -- it is used internally to seed various calculations whose
 * logic would otherwise be circular. */
extern double suntimes_get_approx_sunrise_time (int doy, double longitude);

/** Set get_approx_sunrise_time(). */
extern double suntimes_get_approx_sunset_time (int doy, double longitude);

/** Get the sun's apparent hour angle given its longitude, and the observer's
 * latitude, with respect to the specific zenith. Essentially this is the
 * angle the Sun makes with the zenith at a particular time at the 
 * observer's location.
 *
 * https://en.wikipedia.org/wiki/Hour_angle */
extern double suntimes_get_cos_local_hour_angle (double sun_longitude, 
          double obs_latitude, double zenith);

/** Calculate the sun's mean anomaly as an angle in degrees, at the specified
 * day of year, at a crudely-estimated time of sunset. We use this figure to
 * obtain a better estimate of sunset. For a description of mean anomaly, see
 * https://en.wikipedia.org/wiki/Mean_anomaly. The return value is in degrees.
 *
 * Because the Sun's orbit around the Earth (!) is not particularly eccentric,
 * and because the number of degrees in a circle is nearly the same as days in
 * a year, the result is usually roughly the same as the day of the year, which
 * can be confusing. */ 
extern double suntimes_get_sun_mean_anomaly_at_sunrise 
                  (int doy, double longitude);

/** See get_sun_mean_anomaly_at_sunrise(). */
extern double suntimes_get_sun_mean_anomaly_at_sunset
                  (int doy, double longitude);

/** Get the Sun's right ascension from it celestial longitude. Both
 * input and output are in degrees. */ 
extern double suntimes_get_sun_ra_hours (double longitude);

/** Get the celestial longitude of the sun, at some angle around its orbit. */
extern double suntimes_get_sun_true_longitude (double angle);

/** Get the time of sunrise on the day which includes the specified
 * time. It doesn't matter, in principle, what time the value of
 * t corresponds to -- only the day of the year is actually used.
 * However, we need to specify a complete time, rather than just a day
 * of year, so the returned time_t can be meaningful. */
time_t suntimes_get_sunrise (time_t day, double latitude, double longitude, 
       double zenith);

/** See get_sunrise(). */
time_t suntimes_get_sunset (time_t day, double latitude, double longitude, 
       double zenith);

END_DECLS


