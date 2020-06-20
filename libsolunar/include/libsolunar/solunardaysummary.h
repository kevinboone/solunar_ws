/*============================================================================
  
  libsolunar 
  
  solunardaysummary.h

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

struct _SolunarDaySummary;
typedef struct _SolunarDaySummary SolunarDaySummary;

BEGIN_DECLS

/** Get a summary of the day's solunar events. The day is that which contains
 * the specified date. Since all times, input and output, are in UTC, it's
 * non-obvious why this method needs a timezone. The timezone does not
 * change any of the values returned (since all are time_t's), but it
 * controls the _range_ of times in which moonrises and moonsets are
 * searched. Since it's a _day_ summary, we want the events to take place
 * within 24 hours, in some specific timezone. Pass NULL for tz to use
 * local time. 
 *
 * The city argument is no used in calculations -- it's just stored for
 * future use. This argument can also legitimately be null 
 * The supplied time value is called 'date' to reinforce the fact that
 * only the date is required to work out a daily summary. However, the
 * exact time does have a slight impact, as it used to determine the
 * moon phase and distance. These things won't vary much during a day,
 * but they will vary a little. */
extern SolunarDaySummary *solunar_day_summary_create 
        (time_t date, double latitude, double longitude, const char *city, 
	 const char *tz);

extern void   solunar_day_summary_destroy (SolunarDaySummary *self);

/** Get the city name that was supplied when this object was created. 
 * It might be null, if no name was given. */
extern const char *solunar_day_summary_get_city
                (const SolunarDaySummary *self);

extern time_t solunar_day_summary_get_date (const SolunarDaySummary *self);

extern time_t solunar_day_summary_get_end_civil_twilight 
                 (const SolunarDaySummary *sds);

extern time_t solunar_day_summary_get_end_nautical_twilight 
                 (const SolunarDaySummary *sds);

extern time_t solunar_day_summary_get_end_astronomical_twilight 
                 (const SolunarDaySummary *sds);

extern time_t solunar_day_summary_get_high_noon
                 (const SolunarDaySummary *sds);

extern double solunar_day_summary_get_latitude
                 (const SolunarDaySummary *self);

extern double solunar_day_summary_get_longitude
                 (const SolunarDaySummary *self);

/** Days since new moon. */
extern double solunar_day_summary_get_moon_age 
                 (const SolunarDaySummary *self);

/** Distance from earth to moon in km. */
extern double solunar_day_summary_get_moon_distance 
                 (const SolunarDaySummary *self);

/** Moon phase, in range 0-1. 0 and 1 are both new; full is 0.5. */
extern double solunar_day_summary_get_moon_phase 
                 (const SolunarDaySummary *self);

/** Moon phase name e.g., waxing gibbous. */
extern const char *solunar_day_summary_get_moon_phase_name 
                 (const SolunarDaySummary *self);

/** Get the n'th moonrise event. */
extern time_t solunar_day_summary_get_moon_rise 
                (const SolunarDaySummary *self, int n);

/** Get the n'th moonset event. */
extern time_t solunar_day_summary_get_moon_set
                (const SolunarDaySummary *self, int n);

/** Get numbers of moonrises during the day. There can be 0-2. */
extern int solunar_day_summary_get_n_rises (const SolunarDaySummary *self);

/** Get numbers of moonsets during the day. There can be 0-2. */
extern int solunar_day_summary_get_n_sets (const SolunarDaySummary *self);

extern time_t solunar_day_summary_get_start_civil_twilight 
                 (const SolunarDaySummary *sds);

extern time_t solunar_day_summary_get_start_nautical_twilight 
                 (const SolunarDaySummary *sds);

extern time_t solunar_day_summary_get_start_astronomical_twilight 
                 (const SolunarDaySummary *sds);

/* Get the Sun's altitude above horizon at high noon. */
extern double solunar_day_summary_get_sun_max_altitude 
                 (const SolunarDaySummary *self);

extern time_t solunar_day_summary_get_sunrise (const SolunarDaySummary *self);
extern time_t solunar_day_summary_get_sunset (const SolunarDaySummary *self);

extern KString *solunar_day_summary_to_json (const SolunarDaySummary *self);

END_DECLS

