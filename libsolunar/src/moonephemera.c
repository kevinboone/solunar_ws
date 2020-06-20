
/*============================================================================
  
  libsolunar 
  
  moonephemera.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/moonephemera.h>
#include <libsolunar/astroutil.h>
#include <klib/klog.h>

static const double TWO_PI = 2.0 * M_PI; 

#define KLOG_CLASS "libsolunar.moonephemera"

/** Base date for calculations as a JD (1st Dec 1980). */
const double EPOCH  = 2444238.5;

/** Limiting parameter for the Kepler equation. */
static double KEPSILON = 1.0e-6;

/** Degrees to radians. */
static double DEG_RAD = M_PI / 180.0;

/*
* Constants defining the Sun's orbit (!)
*
*/
/** Ecliptic longitude of the Sun at epoch 1980. */
const double ELONGE = 278.833540;

/** Ecliptic longitude of the Sun at perigee. */
const double ELONGP = 282.596403;

/** Eccentricity of Earth's orbit. */
const double ECCENT = 0.016718;

/** Semi-major axis of Earth's orbit (km). */
const double SUNSMAX = 1.495985e8;

/** Sun's angular size (deg) at semi-major axis distance. */
const double SUNANGSIZ = 0.533128;

/* Moon mean longitude at epoch (deg). */
const double MMLONG = 64.975464;

/** Mean longitude of the perigee at the epoch (deg). */
const double MMLONGP = 349.383063;

/** Mean longitude of the node at the epoch. */
const double MLNODE = 151.950429;

/** Inclination of the Moon's orbit. */
const double MINC = 5.145396;

/** Eccentricity of the Moon's orbit. */
const double MECC = 0.054900;

/** Moon's angular size from Earth at apogee. */
const double MANGSIZE = 0.5181;

/** Semi-major axis of Moon's orbit (km). */
const double MSMAX = 384401.0;

/** Parallax per unit distance from Earth. */
const double MPARALLAX = 0.9507;

/** Synodic month (new Moon to new Moon) (days). */
const double SYNMONTH = 29.53058868;

/** Moon radius (km) */
const double MOONRAD = 1737.4;

/** Base date for E. W. Brown's numbered series of lunations */
const double LUNATBASE = 2423436.0;

/** Radius of Earth in kilometres. */
const double EARTHRAD = 6378.16;


/*============================================================================
  
  moonephemera_get_sin_altitude

  ==========================================================================*/
double moonephemera_get_sin_altitude (double latitude, double longitude, 
          time_t t)
  {
  KLOG_IN
  double ra, dec;
  moonephemera_get_ra_and_dec (t, &ra, &dec);

  double result = astroutil_ra_dec_to_sin_altitude (t, latitude, longitude, 
           ra, dec);

  KLOG_OUT
  return result;
  }


/*============================================================================
  
  moonephemera_kepler

  ==========================================================================*/
static double moonephemera_kepler (double m, double ecc)
  {
  // We're in big trouble if this doesn't converge but, so far, it 
  //  always has. Perhaps for safety there ought to be some kind of
  //  emergency escape?
  m *= DEG_RAD;
  double delta, e = m;
  for (delta = 1.0; fabs(delta) > KEPSILON;)
    {
    delta = e - ecc * sin(e) - m;
    e -= delta / (1 - ecc * cos(e));
    }
  return e;
  }

/*============================================================================
  
  moonephemera_get_phase_name

  ==========================================================================*/
const char *moonephemera_get_phase_name (double phase)
  {
  if (phase < 0 + 0.0625) return "new";
  if (phase < 0.125 + 0.0625) return "waxing crescent";
  if (phase < 0.25 + 0.0625) return "1st quarter";
  if (phase < 0.375 + 0.0625) return "waxing gibbous";
  if (phase < 0.5 + 0.0625) return "full";
  if (phase < 0.625 + 0.0625) return "waning gibbous";
  if (phase < 0.75 + 0.0625) return "3rd quarter";
  if (phase < 0.875 + 0.0625) return "waning crescent";
  return "new";
  }


/*============================================================================
  
  moonephemera_get_moon_state

  ==========================================================================*/
void moonephemera_get_moon_state (double latitude, double longitude, 
         time_t time, const char **phase_name, double *phase, 
         double *age, double *distance)
  {
  double jd = datetimeconv_time_to_jd (time);
  double day = jd - EPOCH; 
 
  double N = mathutil_fix_angle ((360.0 / 365.2422) * day);                  
  double M = mathutil_fix_angle (N + ELONGE - ELONGP);
  double Ec = moonephemera_kepler (M, ECCENT);

  Ec = sqrt ((1.0 + ECCENT) / 
     (1.0 - ECCENT)) * tan (Ec / 2.0);
  Ec = 2.0 * atan (Ec) / DEG_RAD; 

  double Lambdasun = mathutil_fix_angle (Ec + ELONGP);

  // Calculate Moon position 
  double ml = mathutil_fix_angle (13.1763966 * day + MMLONG);        
  double MM = mathutil_fix_angle (ml - 0.1114041 * day - MMLONGP);   
  double Ev = 1.2739 * mathutil_sin_deg (2.0 * (ml - Lambdasun) - MM);     
  double Ae = 0.1858 * mathutil_sin_deg (M);
  double A3 = 0.37 * mathutil_sin_deg (M);
  double MmP = MM + Ev - Ae - A3;
  double mEc = 6.2886 * mathutil_sin_deg (MmP);
  double A4 = 0.214 * mathutil_sin_deg(2 * MmP);
  double lP = ml + Ev + mEc - Ae + A4;
  double V = 0.6583 * mathutil_sin_deg (2 * (lP - Lambdasun));
  double lPP = lP + V;

  // Calculate Moon phase 
  double MoonAgeDegrees = lPP - Lambdasun;
  double MoonDist = (MSMAX * (1.0 - MECC * MECC)) /
       (1.0 + MECC * mathutil_cos_deg (MmP + mEc));

  double MoonFraction = mathutil_fix_angle (MoonAgeDegrees) / 360.0;
  *phase = MoonFraction;
  *age = MoonFraction * SYNMONTH;
  *distance = MoonDist;
  *phase_name = moonephemera_get_phase_name (*phase);
  }


/*============================================================================
  
  moonephemera_get_ra_and_dec

  ==========================================================================*/
void moonephemera_get_ra_and_dec (time_t tu, double *ra, double *dec)
  {
  KLOG_IN
  double mjd = datetimeconv_time_to_mjd (tu);
  const double CosEPS = 0.91748;
  const double SinEPS = 0.39778;
  const double ARC = 206264.8062;

  const double P2 = TWO_PI;
  const double JD = mjd + 2400000.5;
  double t = (JD - 2451545.0)/36525.0;

  double L0 = mathutil_pascal_frac (0.606433 + 1336.855225 * t);
  double L = P2 * mathutil_pascal_frac (0.374897 + 1325.552410 * t);
  double LS = P2 * mathutil_pascal_frac (0.993133 + 99.997361 * t);
  double D = P2 * mathutil_pascal_frac (0.827361 + 1236.853086 * t);
  double F = P2 * mathutil_pascal_frac (0.259086 + 1342.227825 * t);

  double DL =  22640*sin(L) - 4586*sin(L - 2*D)+ 2370 * sin(2*D);
  DL +=  +769 * sin(2*L)  - 668*sin(LS) - 412*sin(2*F);
  DL +=  -212 * sin(2*L - 2*D) - 206*sin(L + LS - 2*D);
  DL +=  +192 * sin(L + 2*D) - 165 * sin(LS - 2*D);
  DL +=  -125 * sin(D) - 110 * sin(L + LS) +148 * sin(L - LS);
  DL +=   -55 * sin(2*F - 2*D);

  double S = F + (DL + 412 * sin(2*F) + 541* sin(LS)) / ARC;
  double H = F - 2*D;
  double N =   -526 * sin(H)+44 * sin(L + H) -31 * sin(-L + H);
  N +=   -23*sin(LS + H) + 11*sin(-LS + H) - 25*sin(-2*L + F);
  N +=   21*sin(-L + F);

  double L_moon = P2 * mathutil_pascal_frac(L0 + DL / 1296000);
  double B_moon = (18520.0 * sin(S) + N) /ARC;
  double CB = cos(B_moon);
  double X = CB * cos(L_moon);
  double V = CB * sin(L_moon);
  double W = sin(B_moon);
  double Y = CosEPS * V - SinEPS * W;
  double Z = SinEPS * V + CosEPS * W;
  double RHO = sqrt(1.0 - Z*Z);
  double _dec = (360.0 / P2) * atan(Z / RHO);
  double _ra = (48.0 / P2) * atan(Y / (X + RHO));

  if (_ra < 0) _ra += 24 ;

  *ra = _ra;
  *dec = _dec;

  KLOG_OUT
  }



