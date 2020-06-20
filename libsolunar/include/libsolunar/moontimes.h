/*============================================================================
  
  libsolunar 
  
  moontimes.h

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/klib.h>


BEGIN_DECLS

/* Determine moonrises in the specified time period, at the specific
 * location. The results are written into an array of time_t values
 * of size max. The number of moonrises, if any, is written to count.
 *
 * The are between 0 and 2 moonrises in any 24-hour period. This function
 * can work over longer periods, but bear in mind that it is iteractive,
 * and computationally expensive. */ 
extern void moontimes_get_moonrises (time_t start, time_t end, 
        double latitude, double longitude, time_t *rises, 
        int max, int *count);

/* See get_moonrises */
extern void moontimes_get_moonsets (time_t start, time_t end, 
        double latitude, double longitude, time_t *rises, 
        int max, int *count);

END_DECLS
