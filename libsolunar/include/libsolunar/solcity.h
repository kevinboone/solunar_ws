/*============================================================================
  
  klib
  
  solcity.h

  A class for managing city-related data (lat/long, etc) and for
  managing the city list

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>

struct _SolCity;
typedef struct _SolCity SolCity;

BEGIN_DECLS

/** Find cities that match the specified name, which can be partial.
 * In most cases, returning more than one city probably represents an
 * error, or insufficient user input. If the function does return a
 * list, the caller should destroy it. The list will contain instances
 * of SolCity */
extern KList *solcity_find_matching (const UTF8 *s);

/** Get the latitude of the city, in degrees, +north. */
extern double solcity_get_latitude (const SolCity *self);

/** Get the logitude of the city, in degrees, +east. */
extern double solcity_get_longitude (const SolCity *self);

/** Get the timezone name, which will usually be the same as the name. */ 
const char *solcity_get_tz_name (const SolCity *self);

/** Get the name, e.g., Europe/London. */
const char *solcity_get_name (const SolCity *self);

END_DECLS


