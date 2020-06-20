/*============================================================================
  
  klib
  
  festival.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <klib/klib.h>
#include <libsolunar/festival.h>

#define KLOG_CLASS "solunar.festival"

static const int SECS_PER_DAY = 24 * 3600;

double periodic24 (double t); //FWD
static char *getenv_dup (const char *env); //FWD
static void my_setenv (const char *name, const char *value, BOOL dummy);

/*============================================================================
  
  Festival 

  ==========================================================================*/
struct _Festival
  {
  time_t  date;
  char   *name;
  BOOL    has_time;
  };

/*============================================================================
  
  festival_new 

  ==========================================================================*/
Festival *festival_new (time_t date, BOOL has_time, const char *name)
  {
  KLOG_IN
  Festival *self = malloc (sizeof (Festival));
  memset (self, 0, sizeof (Festival));
  self->date = date;
  self->has_time = has_time;
  self->name = strdup (name);
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  festival_destroy

  ==========================================================================*/
void festival_destroy (Festival *self)
  { 
  KLOG_IN
  if (self)
    {
    if (self->name) free (self->name);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
  
  festival_get_autumnal_equinox

  ==========================================================================*/
Festival *festival_get_autumnal_equinox (int year)
  {
  KLOG_IN

  double m = (year - 2000.0) / 1000.0;
  double ae = 2451810.21715 + 365242.01767 * m - 0.11575 * m
      * m + 0.00337 * m * m * m + 0.00078 * m * m * m * m ;
  double t = (ae - 2451545.0) / 36525.0;
  double w = 35999.373*t - 2.47;
  double dL = 1 + 0.0334*mathutil_cos_deg(w) + 0.0007*mathutil_cos_deg(2*w);
  double s =periodic24 (t);
  ae = ae + ((0.00001*s) / dL);

  time_t tae = datetimeconv_jd_to_time (ae);

  Festival *ret = festival_new (tae, TRUE, "Autumnal equinox");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  Festival 

  ==========================================================================*/
time_t festival_get_date (const Festival *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->date;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_ash_wednesday

  ==========================================================================*/
Festival *festival_get_ash_wednesday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t -= 46 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Ash Wednesday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  Festival 

  ==========================================================================*/
Festival *festival_get_easter_monday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t += SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Easter Monday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  Festival 

  ==========================================================================*/
Festival *festival_get_easter_sunday (int year, const char *tz)
  {
  KLOG_IN
  int n_year = year;
  int nA = n_year % 19;
  int nB = n_year / 100;
  int nC = n_year % 100;
  int nD = nB / 4;
  int nE = nB % 4;
  int nF = (nB + 8) / 25;
  int nG = (nB - nF + 1) / 3;
  int nH = (19 * nA + nB - nD - nG + 15) % 30;
  int nI = nC / 4;
  int nK = nC % 4;
  int nL = (32 + 2 * nE + 2 * nI - nH - nK) % 7;
  int nM = (nA + 11 * nH + 22 * nL) / 451;
  int n_easter_month = (nH + nL - 7 * nM + 114) / 31;
  --n_easter_month;
  int nP = (nH + nL - 7 * nM + 114) % 31;
  int n_easter_day = nP + 1;
  n_year -= 1900;

  time_t t = datetimeconv_maketime (year, n_easter_month + 1, n_easter_day, 
         2, 0, 0, tz);

  Festival *ret = festival_new (t, FALSE, "Easter Sunday"); 
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_good_friday 

  ==========================================================================*/
Festival *festival_get_good_friday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t -= 2 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Good Friday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_maundy_thursday

  ==========================================================================*/
Festival *festival_get_maundy_thursday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t -= 3 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Maundy Thursday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_mothering_sunday

  ==========================================================================*/
Festival *festival_get_mothering_sunday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t -= 21 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Mothering Sunday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_name

  ==========================================================================*/
const char *festival_get_name (const Festival *self)
  {
  KLOG_IN
  const char *ret = self->name;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
  
  festival_get_palm_sunday

  ==========================================================================*/
Festival *festival_get_palm_sunday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t -= 7 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Palm Sunday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_shrove_tuesday

  ==========================================================================*/
Festival *festival_get_shrove_tuesday (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t -= 47 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Shrove Tuesday");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_summer_solstice

  ==========================================================================*/
Festival *festival_get_summer_solstice (int year, BOOL southern)
  {
  KLOG_IN

  double m = (year - 2000.0) / 1000.0;
  double ss;
  if (southern)
     ss = 2451900.05952 + 365242.74049 * m - 0.06223 * m
      * m - 0.00823 * m * m * m + 0.00032 * m * m * m * m ;
  else
     ss = 2451716.56767 + 365241.62603 * m + 0.00325 * m
      * m + 0.00888 * m * m * m - 0.00030 * m * m * m * m ;
  double t = (ss - 2451545.0) / 36525.0;
  double w = 35999.373*t - 2.47;
  double dL = 1 + 0.0334*mathutil_cos_deg(w) + 0.0007*mathutil_cos_deg(2*w);
  double s = periodic24 (t);
  ss = ss + ((0.00001*s) / dL);

  time_t tss = datetimeconv_jd_to_time (ss);

  Festival *ret = festival_new (tss, TRUE, "Summer solstice");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_vernal_equinox

  ==========================================================================*/
Festival *festival_get_vernal_equinox (int year)
  {
  KLOG_IN

  double m = (year - 2000.0) / 1000.0;
  double ve = 2451623.80984 + 365242.37404 * m + 0.05169 * m
       * m - 0.00411 * m * m * m - 0.00057 * m * m * m * m ;
  double t = (ve - 2451545.0) / 36525.0;
  double w = 35999.373*t - 2.47;
  double dL = 1 + 0.0334*mathutil_cos_deg(w) + 0.0007*mathutil_cos_deg(2*w);
  double s = periodic24 (t);
  ve = ve + ((0.00001*s) / dL);

  time_t tve = datetimeconv_jd_to_time (ve);

  Festival *ret = festival_new (tve, TRUE, "Vernal equinox");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_winter_solstice

  ==========================================================================*/
Festival *festival_get_winter_solstice (int year, BOOL southern)
  {
  KLOG_IN

  double m = (year - 2000.0) / 1000.0;
  double ws;
  if (southern)
    ws = 2451716.56767 + 365241.62603 * m + 0.00325 * m
      * m + 0.00888 * m * m * m - 0.00030 * m * m * m * m ;
  else
    ws = 2451900.05952 + 365242.74049 * m - 0.06223 * m
      * m - 0.00823 * m * m * m + 0.00032 * m * m * m * m ;
  double t = (ws - 2451545.0) / 36525.0;
  double w = 35999.373*t - 2.47;
  double dL = 1 + 0.0334*mathutil_cos_deg(w) + 0.0007*mathutil_cos_deg(2*w);
  double s = periodic24 (t);
  ws = ws + ((0.00001*s) / dL);

  time_t tws = datetimeconv_jd_to_time (ws);

  Festival *ret = festival_new (tws, TRUE, "Winter solstice");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_get_whitsun

  ==========================================================================*/
Festival *festival_get_whitsun (int year, const char *tz)
  {
  KLOG_IN

  Festival *temp = festival_get_easter_sunday (year, tz);
  time_t t = festival_get_date(temp);
  festival_destroy (temp);
  t += 49 * SECS_PER_DAY;
  Festival *ret = festival_new (t, FALSE, "Whitsun/Pentecost");
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_has_time

  ==========================================================================*/
BOOL festival_has_time (const Festival *self)
  {
  KLOG_IN
  BOOL ret = self->has_time;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  festival_to_string 

  ==========================================================================*/
KString *festival_to_string (const Festival *self, const char *tz)
  {
  KLOG_IN
  static char *months[12] = 
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
      "Sep", "Oct", "Nov", "Dec"};
  static char *days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

  char *oldtz = NULL;
  if (tz)
    {
    oldtz = getenv_dup ("TZ");
    my_setenv ("TZ", tz, 1);
    tzset ();
    }

  KString *s = kstring_new_empty ();
  
  struct tm tm;
  localtime_r (&self->date, &tm);  

  kstring_append_printf (s, "%s %s %d %s", 
        days[tm.tm_wday], months[tm.tm_mon], tm.tm_mday, self->name);
  if (self->has_time)
    {
    kstring_append_printf (s, " (%02d:%02d)", tm.tm_hour, tm.tm_min);
    }

  if (tz)
    {
    my_setenv ("TZ", oldtz, 1);
    if (oldtz) free (oldtz);
    tzset ();
    }

  KLOG_OUT
  return s;
  }

/*=======================================================================

  getenv_dup

  Gets a copy of an environment string. Caller must free it, if it is
  not NULL

=======================================================================*/
static char *getenv_dup (const char *env)
  {
  KLOG_IN
  char *s = getenv (env);
  KLOG_OUT
  if (s) return strdup (s);
  return NULL;
  }

/*=======================================================================

  my_setenv

  A variant of setenv() that allows a NULL value to call unsetenv; 
  setenv() itself does not accept NULL as a value

=======================================================================*/
static void my_setenv (const char *name, const char *value, BOOL dummy)
  {
  KLOG_IN
  if (value)
    setenv (name, value, 1);
  else
    unsetenv (name);
  KLOG_OUT
  }

/*============================================================================
  
  periodic24

  Meeus' correction method for solstices. Don't ask me how the math
  works :)

=======================================================================*/
double periodic24 (double t)
  {
  int i;
  static double A[24] = {485,203,199,182,156,136,77,74,70,58,
      52,50,45,44,29,18,17,16,14,12,12,12,9,8};
  static double B[24] = {324.96,337.23,342.08,27.85,73.14,
      171.52,222.54,296.72,243.58,119.81,297.17,21.02, 247.54,
      325.15,60.93,155.12,288.79,198.04,199.76,95.39,287.11,
      320.81,227.73,15.45};
  static double C[24] = {1934.136,32964.467,20.186,445267.112,
      45036.886,22518.443, 65928.934,3034.906,9037.513,33718.147,
      150.678,2281.226, 29929.562,31555.956,4443.417,67555.328,
      4562.452,62894.029, 31436.921,14577.848,31931.756,34777.259,
      1222.114,16859.074};
  double s = 0.0;
  for (i = 0; i < 24; i++)
    s += A[i]*mathutil_cos_deg(B[i] + C[i]*t);
  return s;
  }




