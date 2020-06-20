/*============================================================================
  
  klib
  
  datetimeconv.h 
  
  Functions for converting times and dates 
  
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <time.h>

BEGIN_DECLS

extern char *datetimeconv_format_time (const char *fmt, const char *tz_city, 
         time_t t);

/** Get the day of the year in which falls the specified time. For the
    avoidance of doubt: t relates to a UTC time. */
extern int    datetimeconv_get_day_of_year (time_t t);

/** Convert a julian date to a modified julian date. */
double datetimeconv_jd_to_mjd (double jd);

/** Convert a julian date to a Unix time_t. */
time_t datetimeconv_jd_to_time (double jd);

/** Make a Unix time_t from a list of time values. Any value can be -1,
 * in which case the value for the current time is user. tz is a timezone
 * name, e.g., EST, or a city name, e.g., 'Europe/Paris'. It can't be
 * a timezone name that specifies DST (e.g., EDT). Whether daylight
 * saving is in effect or not is determined automatically from the
 * date */
extern time_t datetimeconv_maketime (int year, int month, int day, 
                int hour, int min, int sec, const char *tz);

/** Change the time of a time_t, whilst keeping the date the same. */
extern time_t datetimeconv_make_time_on_day (time_t t, int h, 
                int m, int s, const char *tz);

/** Parse a date in a variety of different formats. The h and m arguments
 * are the hours and minutes to fill in, to complete the time_t return
 * value. Supported formats are:
 * Jan 21
 * 21 Jan
 * Jan 21 2020
 * Jan 21 20
 * 21 Jan 2020
 * 21 Jan 20
 * 2020-01-21 
 * Month names are not case-sensitive, and can be abbreviated or full.
 * Name are appropriate to the system locale.  Note: no formats containing
 * "/" are accepted -- there's too much confusion beween US and
 * European values. Return value is (time_t)0 if the input cannot be parsed.
 */
time_t datetimeconv_parse_date (const char *s, int h, int m, const char *tz);

/* Convert a unix time to a julian date. */
extern double datetimeconv_time_to_jd (time_t t);

/* Convert a unix time to a modified julian date. */
extern double datetimeconv_time_to_mjd (time_t t);

END_DECLS


