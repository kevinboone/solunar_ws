
/*============================================================================
  
  libsolunar 
  
  solunaryearummary.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/solunaryearsummary.h>
#include <libsolunar/festival.h>
#include <klib/klog.h>

#define KLOG_CLASS "libsolunar.solunaryearsummary"


/*============================================================================
 
  SolunarYearSummary 

  ==========================================================================*/
struct _SolunarYearSummary
  {
  KList *list;
  int year;
  double latitude;
  char *tz;
  };


/*============================================================================
 
  solunar_year_summary_create 

  ==========================================================================*/
SolunarYearSummary *solunar_year_summary_create 
        (int year, double latitude, const char *tz)
  {
  KLOG_IN
  SolunarYearSummary *self = malloc (sizeof (SolunarYearSummary));
  memset (self, 0, sizeof (SolunarYearSummary));

  if (tz)
    self->tz = strdup (tz);
  self->latitude = latitude;
  self->year = year;

  self->list = klist_new_empty ((KListFreeFn) festival_destroy);

  klist_append (self->list, 
    festival_get_shrove_tuesday (year, tz));

  klist_append (self->list, 
    festival_get_ash_wednesday (year, tz));

  klist_append (self->list, 
    festival_get_mothering_sunday (year, tz));

  klist_append (self->list, 
    festival_get_palm_sunday (year, tz));

  klist_append (self->list, 
    festival_get_maundy_thursday (year, tz));

  klist_append (self->list, 
    festival_get_good_friday (year, tz));

  klist_append (self->list, 
    festival_get_easter_sunday (year, tz));

  klist_append (self->list, 
    festival_get_easter_monday (year, tz));

  klist_append (self->list, 
    festival_get_whitsun (year, tz));

  klist_append (self->list, 
    festival_get_vernal_equinox (year));

  klist_append (self->list, 
    festival_get_summer_solstice (year, FALSE));

  klist_append (self->list, 
    festival_get_autumnal_equinox (year));

  klist_append (self->list, 
    festival_get_winter_solstice (year, FALSE));
  // TODO sort

  KLOG_OUT
  return self;
  }

/*============================================================================
 
  solunar_year_summary_destroy

  ==========================================================================*/
void solunar_year_summary_destroy (SolunarYearSummary *self)
  {
  KLOG_IN
  if (self)
    {
    if (self->list) klist_destroy (self->list);
    if (self->tz) free (self->tz);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
 
  solunar_year_summary_to_json

  ==========================================================================*/
KString *solunar_year_summary_to_json (const SolunarYearSummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  KString *json = kstring_new_empty();
  int l = klist_length (self->list); 
  kstring_append_utf8 (json, (UTF8 *)"[");
  for (int i = 0; i < l; i++)
    {
    Festival *f = klist_get (self->list, i);
    kstring_append_utf8 (json, (UTF8 *)"{");
    const char *name = festival_get_name (f);
    time_t date = festival_get_date (f);

    struct tm tm;
    localtime_r (&date, &tm);  

    KString *ds = kstring_new_empty();
    kstring_append_printf (ds, "%04d-%02d-%02d", 
      tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); 
        
    if (festival_has_time (f))
      {
      kstring_append_printf (ds, " (%02d:%02d)", tm.tm_hour, tm.tm_min);
      }

    kstring_append_printf (json, "\"name\":\"%s\",", name); 
    kstring_append_utf8 (json, (UTF8 *)"\"date\":\"");
    kstring_append (json, ds); 
    kstring_append_utf8 (json, (UTF8 *)"\"");
    kstring_destroy (ds);
    kstring_append_utf8 (json, (UTF8 *)"}");
    if (i != l - 1)
      kstring_append_utf8 (json, (UTF8 *)",\n");
    }

  kstring_append_utf8 (json, (UTF8 *)"]\n");
  KLOG_OUT
  return json;
  }

/*============================================================================
 
  solunar_year_summary_to_string

  ==========================================================================*/
KString *solunar_year_summary_to_string (const SolunarYearSummary *self)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->list != NULL);
  KString *s = kstring_new_empty();
  
  int l = klist_length (self->list); 
  for (int i = 0; i < l; i++)
    {
    Festival *f = klist_get (self->list, i);
    KString *ss = festival_to_string (f, self->tz);
    kstring_append (s, ss); 
    kstring_append_utf8 (s, (UTF8 *)"\n"); 
    kstring_destroy (ss);
    }

  KLOG_OUT
  return s;
  }

