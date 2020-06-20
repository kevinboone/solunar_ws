/*==========================================================================

  klib

  datetimeconv.c

  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <klib/klog.h> 
#include <klib/datetimeconv.h> 

#define KLOG_CLASS "klib.datetimeconv"

static char *getenv_dup (const char *env); //FWD
static void my_setenv (const char *name, const char *value, BOOL dummy); // FWD
extern char *strptime (const char *s, const char *fmt, struct tm *tm);


/*==========================================================================

  datetimeconv_format_time

==========================================================================*/
char *datetimeconv_format_time (const char *fmt, const char *tz, 
         time_t t)
  {
  KLOG_IN
  char *oldtz = NULL;
  if (tz)
    {
    oldtz = getenv_dup ("TZ");
    my_setenv ("TZ", tz, 1);
    tzset ();
    }

  char s[100]; 
  struct tm tm;
  localtime_r (&t, &tm);
  if (strcmp (fmt, "24hr") == 0)
    sprintf (s, "%02d:%02d", tm.tm_hour, tm.tm_min);
  else if (strcmp (fmt, "short_date") == 0)
    {
    ctime_r (&t, s);
    memmove (s, s+4, 20);
    s[6] = 0;
    }
  else
    strftime (s, sizeof (s), fmt, &tm);

  if (tz)
    {
    my_setenv ("TZ", oldtz, 1);
    if (oldtz) free (oldtz);
    tzset ();
    }

  KLOG_OUT
  return strdup (s);
  }


/*==========================================================================

  datetimeconv_get_day_of_year

==========================================================================*/
int datetimeconv_get_day_of_year (time_t t)
  {
  KLOG_IN
  struct tm tm;
  memcpy (&tm, gmtime (&t), sizeof (struct tm));
  int ret = tm.tm_yday + 1;
  KLOG_OUT
  return ret;
  }

/*=======================================================================

  datetimeconv_jd_to_mjd

=======================================================================*/
double datetimeconv_jd_to_mjd (double jd)
  {
  KLOG_IN
  double ret = jd - 2400000.5;
  KLOG_OUT
  return ret;
  }


/*==========================================================================

  datetimeconv_maketime

==========================================================================*/
time_t datetimeconv_maketime (int year, int month, int day, 
         int hour, int min, int sec, const char *tz)
  {
  KLOG_IN
  char *oldtz = NULL;
  if (tz)
    {
    oldtz = getenv_dup ("TZ");
    my_setenv ("TZ", tz, 1);
    tzset ();
    }

  time_t now = time (NULL);
  struct tm tm;
  localtime_r (&now, &tm);  

  if (sec >= 0) 
    tm.tm_sec = sec;

  if (min >= 0) 
    tm.tm_min = min;

  if (hour >= 0) 
    tm.tm_hour = hour;

  if (day >= 0) 
    tm.tm_mday = day;

  if (month >= 0) 
    tm.tm_mon = month - 1;

  if (year >= 0) 
    tm.tm_year = year - 1900;

  tm.tm_isdst = -1; // Have the std library work it out

  time_t ret = mktime (&tm); 

  if (tz)
    {
    my_setenv ("TZ", oldtz, 1);
    if (oldtz) free (oldtz);
    tzset ();
    }

  KLOG_OUT
  return ret;
  }

/*==========================================================================

  datetimeconv_maketime_on_day

==========================================================================*/
time_t datetimeconv_make_time_on_day (time_t t, int h, 
                int m, int s, const char *tz)
  {
  KLOG_IN
  char *oldtz = NULL;
  if (tz)
    {
    oldtz = getenv_dup ("TZ");
    my_setenv ("TZ", tz, 1);
    tzset ();
    }

  struct tm tm;
  localtime_r (&t, &tm);  

  if (s >= 0) 
    tm.tm_sec = s;

  if (m >= 0) 
    tm.tm_min = m;

  if (h >= 0) 
    tm.tm_hour = h;

  tm.tm_isdst = -1; // Have the std library work it out

  time_t ret = mktime (&tm); 

  if (tz)
    {
    my_setenv ("TZ", oldtz, 1);
    if (oldtz) free (oldtz);
    tzset ();
    }

  KLOG_OUT
  return ret;
  }


/*=======================================================================

  datetimeconv_parse_date_fmt

=======================================================================*/
BOOL datetimeconv_parse_date_fmt (const char *fmt, const char *s,
          struct tm *tm)
  {
  KLOG_IN
  BOOL ret = FALSE;
  char *last = strptime (s, fmt, tm);
  if (last == s + strlen (s))
    ret = TRUE;
 
  KLOG_OUT
  return ret;
  }

/*=======================================================================

  datetimeconv_parse_date

=======================================================================*/
time_t datetimeconv_parse_date (const char *s, int h, int m, 
         const char *tz)
  {
  KLOG_IN
  time_t ret = (time_t)0;
  BOOL found = FALSE;
  struct tm tm;
  time_t now = time(NULL);

  char *oldtz = NULL;
  if (tz)
    {
    oldtz = getenv_dup ("TZ");
    my_setenv ("TZ", tz, 1);
    tzset ();
    }

  localtime_r (&now, &tm); // We only want the year from this conversion
  tm.tm_hour = h;
  tm.tm_min = m;
  tm.tm_sec = 0;
  tm.tm_isdst = -1;
  if (!found) found = datetimeconv_parse_date_fmt ("%b %e", s, &tm);
  if (!found) found = datetimeconv_parse_date_fmt ("%b %e %Y", s, &tm);
  if (!found) found = datetimeconv_parse_date_fmt ("%e %b", s, &tm);
  if (!found) found = datetimeconv_parse_date_fmt ("%e %b %Y", s, &tm);
  if (!found) found = datetimeconv_parse_date_fmt ("%Y-%m-%e", s, &tm);

  if (found)
    {
    // Good for the next 30 years. I won't be worried by then ;)
    if (tm.tm_year < 50) tm.tm_year += 2000;
    ret = mktime (&tm); 
    }

  if (tz)
    {
    my_setenv ("TZ", oldtz, 1);
    if (oldtz) free (oldtz);
    tzset ();
    }

  KLOG_OUT
  return ret;
  }

/*=======================================================================

  datetimeconv_time_to_jd

=======================================================================*/
double datetimeconv_time_to_jd (time_t t)
  {
  KLOG_IN
  double ret = ((double)t / 86400.0) + 2440587.5; 
  KLOG_OUT
  return ret;
  }

/*=======================================================================

  datetimeconv_jd_to_time

=======================================================================*/
time_t datetimeconv_jd_to_time (double jd)
  {
  KLOG_IN
  time_t ret = 86400.0 * (jd - 2440587.5); 
  KLOG_OUT
  return ret;
  }

/*=======================================================================

  datetimeconv_time_to_mjd

=======================================================================*/
double datetimeconv_time_to_mjd (time_t t)
  {
  KLOG_IN
  double mjd = datetimeconv_jd_to_mjd (datetimeconv_time_to_jd (t));
  KLOG_OUT
  return mjd;
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

