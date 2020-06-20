/*============================================================================
  
  libsolunar 
  
  suntimes.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/suntimes.h>
#include <libsolunar/astroutil.h>
#include <klib/klog.h>

static const double DEG_PER_HOUR = 360.0 / 24.0;
static const double TWO_PI = 2.0 * M_PI; 

#define KLOG_CLASS "libsolunar.suntimes"

/*============================================================================
  
  suntimes_get_approx_sunrise_time

  ==========================================================================*/
double suntimes_get_approx_sunrise_time (int doy, double longitude)
  {
  KLOG_IN
  double ret = doy + ((6.0 - 
    astroutil_get_hours_from_meridian (longitude)) / 24);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_approx_sunset_time

  ==========================================================================*/
double suntimes_get_approx_sunset_time (int doy, double longitude)
  {
  KLOG_IN
  double ret = doy + ((18.0 - 
    astroutil_get_hours_from_meridian (longitude)) / 24);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_local_mean_time

  ==========================================================================*/
static double suntimes_get_local_mean_time (double local_hour, double
    right_ascension_hours, double approx_time_days)
  {
  KLOG_IN
  double ret = local_hour + right_ascension_hours 
     - (0.06571 * approx_time_days) - 6.622;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_cos_local_hour_angle

  ==========================================================================*/
double suntimes_get_cos_local_hour_angle (double sun_longitude, 
          double obs_latitude, double zenith)
  {
  KLOG_IN
  double sin_dec = 0.39782 * mathutil_sin_deg (sun_longitude);
  double cos_dec = mathutil_cos_deg (mathutil_asin_deg (sin_dec));

  double ret = (mathutil_cos_deg(zenith) - 
     (sin_dec * mathutil_sin_deg (obs_latitude))) 
        / (cos_dec * mathutil_cos_deg(obs_latitude));

  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  suntimes_get_sun_mean_anomaly_at_sunrise

  ==========================================================================*/
double suntimes_get_sun_mean_anomaly_at_sunrise (int doy, double longitude)
  {
  KLOG_IN
  double ret = (0.9856 * suntimes_get_approx_sunrise_time 
     (doy, longitude)) - 3.289; 
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_sun_mean_anomaly_at_sunset

  ==========================================================================*/
double suntimes_get_sun_mean_anomaly_at_sunset (int doy, double longitude)
  {
  KLOG_IN
  double ret = (0.9856 * suntimes_get_approx_sunset_time 
     (doy, longitude)) - 3.289; 
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_sunrise

  ==========================================================================*/
time_t suntimes_get_sunrise (time_t day, double latitude, double longitude, 
       double zenith)
  {
  KLOG_IN

  struct tm tm_day;
  gmtime_r (&day, &tm_day);
  int doy = tm_day.tm_yday + 1;

  time_t ret = (time_t)0; // Let's hope that the Sun doesn't set in
                          //  1970 again ;)

  double sma = suntimes_get_sun_mean_anomaly_at_sunrise (doy, longitude);

  double stl = suntimes_get_sun_true_longitude (sma);

  double rah = suntimes_get_sun_ra_hours (stl);

  double clha = suntimes_get_cos_local_hour_angle (stl, latitude, zenith);

  if (clha >= -1 && clha <= 1)
    {
    double lha = 360.0 - mathutil_acos_deg (clha);
    double lh = lha / DEG_PER_HOUR;
    double lmt = suntimes_get_local_mean_time (lh, rah, 
      suntimes_get_approx_sunrise_time (doy, 
         astroutil_get_hours_from_meridian (longitude)));

    double temp = lmt - astroutil_get_hours_from_meridian (longitude);
    if (temp < 0) temp += 24;
    if (temp > 24) temp -= 24;

    int utc_h = (int) temp;
    int utc_m = (int) ((temp - utc_h) * 60);

    tm_day.tm_hour = utc_h;
    tm_day.tm_min = utc_m;
    tm_day.tm_sec = 0;

    ret = timegm (&tm_day);
    }

  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_sunset

  ==========================================================================*/
time_t suntimes_get_sunset (time_t day, double latitude, double longitude, 
       double zenith)
  {
  KLOG_IN

  struct tm tm_day;
  gmtime_r (&day, &tm_day);
  int doy = tm_day.tm_yday + 1;

  time_t ret = (time_t)0; // Let's hope that the Sun doesn't set in
                          //  1970 again ;)

  double sma = suntimes_get_sun_mean_anomaly_at_sunset (doy, longitude);

  double stl = suntimes_get_sun_true_longitude (sma);

  double rah = suntimes_get_sun_ra_hours (stl);

  double clha = suntimes_get_cos_local_hour_angle (stl, latitude, zenith);

  if (clha >= -1 && clha <= 1)
    {
    double lha = mathutil_acos_deg (clha);
    double lh = lha / DEG_PER_HOUR;
    double lmt = suntimes_get_local_mean_time (lh, rah, 
      suntimes_get_approx_sunset_time (doy, 
         astroutil_get_hours_from_meridian (longitude)));

    double temp = lmt - astroutil_get_hours_from_meridian (longitude);
    if (temp < 0) temp += 24;
    if (temp > 24) temp -= 24;

    int utc_h = (int) temp;
    int utc_m = (int) ((temp - utc_h) * 60);

    tm_day.tm_hour = utc_h;
    tm_day.tm_min = utc_m;
    tm_day.tm_sec = 0;

    ret = timegm (&tm_day);
    }

  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  suntimes_get_sun_ra_hours

  ==========================================================================*/
double suntimes_get_sun_ra_hours (double longitude)
  {
  KLOG_IN
  double a = 0.91764 * mathutil_tan_deg (longitude);
  double ra = 360.0 / TWO_PI * atan(a);

  double lQuadrant  = floor (longitude/90.0) * 90.0;
  double raQuadrant = floor (ra/90.0) * 90.0;
  ra = ra + (lQuadrant - raQuadrant);

  double ret = ra/DEG_PER_HOUR; //convert to hours
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  suntimes_get_sun_true_longitude

  ==========================================================================*/
double suntimes_get_sun_true_longitude (double angle)
  {
  KLOG_IN

  double l = angle + (1.916 * mathutil_sin_deg (angle))
                + (0.020 * mathutil_sin_deg (2 * angle)) + 282.634;

  // get longitude into 0-360 degree range
  if (l >= 360.0) l = l - 360.0;
  if (l < 0) l = l + 360.0;

  KLOG_OUT
  return l;
  }

