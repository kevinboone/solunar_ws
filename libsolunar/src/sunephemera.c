
/*============================================================================
  
  libsolunar 
  
  sunephemera.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/sunephemera.h>
#include <libsolunar/astroutil.h>
#include <klib/klog.h>

static const double TWO_PI = 2.0 * M_PI; 

#define KLOG_CLASS "libsolunar.sunephemera"


/*============================================================================
  
  sunephemera_get_sin_altitude

  ==========================================================================*/
double sunephemera_get_sin_altitude (double latitude, double longitude, 
          time_t t)
  {
  KLOG_IN
  double ra, dec;
  sunephemera_get_ra_and_dec (t, &ra, &dec);
  double result = astroutil_ra_dec_to_sin_altitude (t, latitude, longitude, 
           ra, dec);
  KLOG_OUT
  return result;
  }


/*============================================================================
  
  sunephemera_get_ra_and_dec

  ==========================================================================*/
void sunephemera_get_ra_and_dec (time_t t, double *ra, double *dec)
  {
  KLOG_IN
  double mjd = datetimeconv_time_to_mjd (t);
  const double CosEPS = 0.91748;
  const double SinEPS = 0.39778;
  double JD = mjd + 2400000.5;
  double T = (JD - 2451545.0)/36525.0;
  double P2 = TWO_PI;
  double M = P2 * mathutil_pascal_frac (0.993133 + 99.997361 * T);
  double DL = 6893.0 * sin(M) + 72.0 * sin(M * 2.0);
  double L = P2 * mathutil_pascal_frac ( 0.7859453 + M / P2
         + (6191.2 * T + DL) / 1296e3);
  double SL = sin(L);
  double X = cos(L);
  double Y = CosEPS * SL;
  double Z = SinEPS * SL;
  double RHO = sqrt(1.0 - Z * Z);
  *dec = (360.0 / P2) * atan2(Z, RHO);
  *ra = ( 48.0 / P2) * atan2(Y, (X + RHO));
  if (*ra < 0.0)
    {
    *ra += 24.0;
    }
  KLOG_OUT
  }


