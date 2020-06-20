
/*============================================================================
  
  libsolunar 
  
  astroutil.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/astroutil.h>
#include <klib/klog.h>

#define KLOG_CLASS "libsolunar.astroutil"

static const double DEG_PER_HOUR = 360.0 / 24.0;

/*============================================================================
  
  astroutil_get_hours_from_meridian

  ==========================================================================*/
double astroutil_get_hours_from_meridian (double longitude)
  {
  KLOG_IN
  double ret = longitude / DEG_PER_HOUR;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  astroutil_lmst

  ==========================================================================*/
double astroutil_lmst (time_t t, double longitude)
  {
  KLOG_IN
  double mjd = datetimeconv_time_to_mjd (t);
  double MJD0 = floor(mjd);
  double UT = (mjd - MJD0) * 24.0;
  double T = (MJD0 - 51544.5) / 36525.0;
  double GMST = 6.697374558
        + 1.0027379093 * UT
        + (8640184.812866 + (0.093104 - 6.2E-6 * T) * T) * T / 3600.0;
  double LMST = 24.0 * mathutil_pascal_frac ((GMST + longitude / 15.0) / 24.0);
  KLOG_OUT
  return LMST;
  } 

/*============================================================================
  
  astroutil_lmst

  ==========================================================================*/
double astroutil_ra_dec_to_sin_altitude (time_t t, double latitude, 
         double longitude, double ra, double dec)
  { 
  KLOG_IN
  double cos_latitude = mathutil_cos_deg (latitude);
  double sin_latitude = mathutil_sin_deg (latitude);
  double tau = 15.0 * (astroutil_lmst (t, longitude) - ra);
  double result = sin_latitude * mathutil_sin_deg (dec)
                + cos_latitude * mathutil_cos_deg (dec) 
                     * mathutil_cos_deg (tau);
  KLOG_OUT
  return result;
  }


