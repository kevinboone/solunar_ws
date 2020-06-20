
/*============================================================================
  
  libsolunar 
  
  solunardaysummary.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/solunardaysummary.h>
#include <libsolunar/suntimes.h>
#include <libsolunar/sunephemera.h>
#include <libsolunar/moontimes.h>
#include <libsolunar/moonephemera.h>
#include <klib/klog.h>

#define KLOG_CLASS "libsolunar.solunardaysummary"

// Largest number of sunrises and sunsets (each) we can store
#define N_MOON_EVENTS 3

/*============================================================================
 
  SolunarDaySummary 

  ==========================================================================*/
struct _SolunarDaySummary
  {
  time_t sunrise;
  time_t sunset;
  time_t end_civil_twilight;
  time_t start_civil_twilight;
  time_t end_nautical_twilight;
  time_t start_nautical_twilight;
  time_t end_astronomical_twilight;
  time_t start_astronomical_twilight;
  time_t high_noon;
  int nrises;
  int nsets;
  time_t moonrises[N_MOON_EVENTS];
  time_t moonsets[N_MOON_EVENTS];
  double sun_max_altitude;
  double moon_distance; // km
  double moon_phase; // 0-1
  double moon_age; // Days since new
  const char *moon_phase_name;
  char *city;
  char *tz_city;
  double longitude;
  double latitude;
  time_t date;
  };


/*============================================================================
 
  solunar_day_summary_create 

  ==========================================================================*/
SolunarDaySummary *solunar_day_summary_create 
        (time_t date, double latitude, double longitude, const char *city, 
	  const char *tz)
  {
  KLOG_IN
  SolunarDaySummary *self = malloc (sizeof (SolunarDaySummary));
  memset (self, 0, sizeof (SolunarDaySummary));

  self->longitude = longitude;
  self->latitude = latitude;
  self->date = date;

  self->sunrise = suntimes_get_sunrise 
	  (date, latitude, longitude, SUNTIMES_DEFAULT_ZENITH);

  self->sunset = suntimes_get_sunset
	  (date, latitude, longitude, SUNTIMES_DEFAULT_ZENITH);

  self->end_civil_twilight = suntimes_get_sunset
	  (date, latitude, longitude, SUNTIMES_CIVIL_TWILIGHT);

  self->end_nautical_twilight = suntimes_get_sunset
	  (date, latitude, longitude, SUNTIMES_NAUTICAL_TWILIGHT);

  self->end_astronomical_twilight = suntimes_get_sunset
	  (date, latitude, longitude, SUNTIMES_ASTRONOMICAL_TWILIGHT);

  self->start_civil_twilight = suntimes_get_sunrise
	  (date, latitude, longitude, SUNTIMES_CIVIL_TWILIGHT);

  self->start_nautical_twilight = suntimes_get_sunrise
	  (date, latitude, longitude, SUNTIMES_NAUTICAL_TWILIGHT);

  self->start_astronomical_twilight = suntimes_get_sunrise
	  (date, latitude, longitude, SUNTIMES_ASTRONOMICAL_TWILIGHT);

  time_t tstart = datetimeconv_make_time_on_day (date, 0, 0, 0, tz);
  time_t tend = datetimeconv_make_time_on_day (date, 23, 59, 0, tz);

  moontimes_get_moonrises (tstart, tend, latitude, longitude, 
    self->moonrises, N_MOON_EVENTS, &self->nrises); 
  moontimes_get_moonsets (tstart, tend, latitude, longitude, 
    self->moonsets, N_MOON_EVENTS, &self->nsets); 
  
  // In principle, this calculation should take into account the
  //  fact that the Earth moves in its orbit between sunrise and
  //  sunset. It's not as simple a calculation as this. But the
  //  difference is only a minute or so. For better results, we
  //  could work find the maximum of the sine altitude over 
  //  a period say, ten mintes each side of this estimate. But
  //  it's a lot of work for little gain.
  self->high_noon = (self->sunrise + self->sunset) / 2;

  double sma = sunephemera_get_sin_altitude (latitude, longitude, 
          self->high_noon);
  if (sma > -1 && sma < 1)
    self->sun_max_altitude = mathutil_asin_deg (sma);
  else
    klog_warn (KLOG_CLASS, "Sun sine altitude not in range -1..1");

  moonephemera_get_moon_state (latitude, longitude, date, 
       &self->moon_phase_name, &self->moon_phase, &self->moon_age, 
       &self->moon_distance);

  if (tz) self->tz_city = strdup (tz);
  if (city) self->city = strdup (city);

  KLOG_OUT
  return self;
  }

/*============================================================================
 
  solunar_day_summary_destroy

  ==========================================================================*/
void solunar_day_summary_destroy (SolunarDaySummary *self)
  {
  KLOG_IN
  if (self)
    {
    if (self->tz_city) free (self->tz_city);
    if (self->city) free (self->city);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
 
  solunar_day_summary_get_city

  ==========================================================================*/
const char *solunar_day_summary_get_city
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  const char *ret = self->city;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_date

  ==========================================================================*/
time_t solunar_day_summary_get_date (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->date;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_end_astronomical_twilight

  ==========================================================================*/
time_t solunar_day_summary_get_end_astronomical_twilight
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->end_astronomical_twilight;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_end_civil_twilight

  ==========================================================================*/
time_t solunar_day_summary_get_end_civil_twilight
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->end_civil_twilight;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_end_nautical_twilight

  ==========================================================================*/
time_t solunar_day_summary_get_end_nautical_twilight
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->end_nautical_twilight;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_latitude

  ==========================================================================*/
extern double solunar_day_summary_get_latitude
                 (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  double ret = self->latitude; 
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_longitude

  ==========================================================================*/
extern double solunar_day_summary_get_longitude
                 (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  double ret = self->longitude; 
  KLOG_OUT
  return ret; 
  }


/*============================================================================
 
  solunar_day_summary_get_high_noon

  ==========================================================================*/
time_t solunar_day_summary_get_high_noon
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->high_noon;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_moon_age

  ==========================================================================*/
double solunar_day_summary_get_moon_age (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  double ret = self->moon_age;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_moon_distance

  ==========================================================================*/
double solunar_day_summary_get_moon_distance (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  double ret = self->moon_distance;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_moon_phase

  ==========================================================================*/
double solunar_day_summary_get_moon_phase (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  double ret = self->moon_phase;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_moon_phase_name

  ==========================================================================*/
const char *solunar_day_summary_get_moon_phase_name 
        (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  const char *ret = self->moon_phase_name;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_moon_rise

  ==========================================================================*/
extern time_t solunar_day_summary_get_moon_rise 
                (const SolunarDaySummary *self, int n)
  {
  KLOG_IN
  assert (self != NULL);
  assert (n < self->nrises);
  time_t ret = self->moonrises[n];
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_moon_set

  ==========================================================================*/
time_t solunar_day_summary_get_moon_set
                (const SolunarDaySummary *self, int n)
  {
  KLOG_IN
  assert (self != NULL);
  assert (n < self->nsets);
  time_t ret = self->moonsets[n];
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_n_rises

  ==========================================================================*/
int solunar_day_summary_get_n_rises (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->nrises;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_n_sets

  ==========================================================================*/
int solunar_day_summary_get_n_sets (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->nsets;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_sunrise

  ==========================================================================*/
time_t solunar_day_summary_get_sunrise (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->sunrise;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_sunset

  ==========================================================================*/
time_t solunar_day_summary_get_sunset (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->sunset;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_start_astronomical_twilight

  ==========================================================================*/
time_t solunar_day_summary_get_start_astronomical_twilight
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->start_astronomical_twilight;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_start_civil_twilight

  ==========================================================================*/
time_t solunar_day_summary_get_start_civil_twilight
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->start_civil_twilight;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_start_nautical_twilight

  ==========================================================================*/
time_t solunar_day_summary_get_start_nautical_twilight
                (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  time_t ret = self->start_nautical_twilight;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_get_start_nautical_twilight

  ==========================================================================*/
extern double solunar_day_summary_get_sun_max_altitude 
                 (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  double ret = self->sun_max_altitude;
  KLOG_OUT
  return ret; 
  }

/*============================================================================
 
  solunar_day_summary_to_json

  ==========================================================================*/
KString *solunar_day_summary_to_json (const SolunarDaySummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  KString *json = kstring_new_empty(); 
  kstring_append_printf (json, "{");

  const char *city = self->city;
  if (city)
    kstring_append_printf (json, "\"city\":\"%s\",\n", city);
  const char *tz_city = self->tz_city;
  kstring_append_printf (json, "\"timezone city\":\"%s\",\n", tz_city);
  kstring_append_printf (json, "\"latitude\":%g,\n", self->latitude);
  char *s = datetimeconv_format_time ("short_date", tz_city, self->date);
  kstring_append_printf (json, "\"longitude\":%g,\n", self->longitude);
  kstring_append_printf (json, "\"date\":\"%s\",\n", s);
  free (s);
  kstring_append_printf (json, "\"sun\":{");

  if (self->sunrise)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, self->sunrise);
    kstring_append_printf (json, "\"sunrise\":\"%s\",\n", s);
    free (s);
    }
  if (self->sunset)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, self->sunset);
    kstring_append_printf (json, "\"sunset\":\"%s\",\n", s);
    free (s);
    }
  if (self->start_civil_twilight)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->start_civil_twilight);
    kstring_append_printf (json, "\"start civil twilight\":\"%s\",\n", s);
    free (s);
    }
  if (self->end_civil_twilight)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->end_civil_twilight);
    kstring_append_printf (json, "\"end civil twilight\":\"%s\",\n", s);
    free (s);
    }
  if (self->start_nautical_twilight)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->start_nautical_twilight);
    kstring_append_printf (json, "\"start nautical twilight\":\"%s\",\n", s);
    free (s);
    }
  if (self->end_nautical_twilight)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->end_nautical_twilight);
    kstring_append_printf (json, "\"end nautical twilight\":\"%s\",\n", s);
    free (s);
    }
  if (self->start_astronomical_twilight)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->start_astronomical_twilight);
    kstring_append_printf (json, "\"start astronomical twilight\":\"%s\",\n", s);
    free (s);
    }
  if (self->end_astronomical_twilight)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->end_astronomical_twilight);
    kstring_append_printf (json, "\"end astronomical twilight\":\"%s\",\n", s);
    free (s);
    }
  if (self->high_noon)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
      self->high_noon);
    kstring_append_printf (json, "\"high noon\":\"%s\",\n", s);
    free (s);
    kstring_append_printf (json, "\"sun altitude at high noon\":%g,\n", 
       self->sun_max_altitude);
    }

  if (kstring_ends_with_utf8 (json, (UTF8 *)",\n"))
    {
    int l = kstring_length (json);
    kstring_delete (json, l - 2, 2);
    }

  kstring_append_printf (json, "},\n");

  kstring_append_printf (json, "\"moon\":{");

  kstring_append_printf (json, "\"rises\":[");
  int nrises = self->nrises; 
  for (int i = 0; i < nrises; i++)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
	   self->moonrises[i]); 
    kstring_append_printf (json, "\"%s\",", s);
    free (s);
    }

  if (kstring_ends_with_utf8 (json, (UTF8 *)","))
    {
    int l = kstring_length (json);
    kstring_delete (json, l - 1, 1);
    }
  kstring_append_printf (json, "],\n");

  kstring_append_printf (json, "\"sets\":[");
  int nsets = self->nsets; 
  for (int i = 0; i < nsets; i++)
    {
    char *s = datetimeconv_format_time ("24hr", tz_city, 
	   self->moonsets[i]); 
    kstring_append_printf (json, "\"%s\",", s);
    free (s);
    }

  if (kstring_ends_with_utf8 (json, (UTF8 *)","))
    {
    int l = kstring_length (json);
    kstring_delete (json, l - 1, 1);
    }
  kstring_append_printf (json, "],\n");
  kstring_append_printf (json, "\"moon phase name\":\"%s\",\n", self->moon_phase_name);
  kstring_append_printf (json, "\"moon phase\":%g,\n", self->moon_phase);
  kstring_append_printf (json, "\"moon age\":%g\n", self->moon_age);

  kstring_append_printf (json, "}");
  kstring_append_printf (json, "}");
  KLOG_OUT
  return json; 
  }




