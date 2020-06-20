/*============================================================================
  
  klib
  
  festival.h

  A class for finding and managing festival dates with astronomical
  significance, like Easter

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

struct _Festival;
typedef struct _Festival Festival;

BEGIN_DECLS

extern void festival_destroy (Festival *self);

extern time_t festival_get_date (const Festival *self);
extern const char *festival_get_name (const Festival *self);

extern Festival *festival_get_autumnal_equinox (int year);
extern Festival *festival_get_ash_wednesday (int year, const char *tz);
extern Festival *festival_get_easter_sunday (int year, const char *tz);
extern Festival *festival_get_easter_monday (int year, const char *tz);
extern Festival *festival_get_good_friday (int year, const char *tz);
extern Festival *festival_get_maundy_thursday (int year, const char *tz);
extern Festival *festival_get_mothering_sunday (int year, const char *tz);
extern Festival *festival_get_palm_sunday (int year, const char *tz);
extern Festival *festival_get_shrove_tuesday (int year, const char *tz);
extern Festival *festival_get_summer_solstice (int year, BOOL southern);
extern Festival *festival_get_whitsun (int year, const char *tz);
extern Festival *festival_get_vernal_equinox (int year);
extern Festival *festival_get_winter_solstice (int year, BOOL southern);

extern BOOL festival_has_time (const Festival *self);
extern KString *festival_to_string (const Festival *self, const char *tz);

END_DECLS


