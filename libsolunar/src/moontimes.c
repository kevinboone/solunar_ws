/*============================================================================
  
  libsolunar 
  
  moontimes.h 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <libsolunar/moonephemera.h>
#include <libsolunar/moontimes.h>
#include <libsolunar/astroutil.h>
#include <klib/klog.h>

//static const double DEG_PER_HOUR = 360.0 / 24.0;
//static const double TWO_PI = 2.0 * M_PI; 

#define KLOG_CLASS "libsolunar.moontimes"

#define INTERVAL (15*60)

/*============================================================================
  
  moontimes_get_moonrises

  ==========================================================================*/
void moontimes_get_moonrises (time_t start, time_t end, double latitude, 
      double longitude, time_t *rises, int max, int *count) 
  {
  KLOG_IN
  assert (end > start);
  int diff = end - start;
  int npoints = diff / INTERVAL + 1;
  *count = 0;

  double *x = (double *) malloc (npoints * sizeof (double));
  double *y = (double *) malloc (npoints * sizeof (double));
  double *d_events = malloc (max * sizeof (double));

  time_t tx = start;
  for (int i = 0; i < npoints; i++)
    {
    double alt = moonephemera_get_sin_altitude
      (latitude, longitude, tx);
    x[i] = i * INTERVAL;
    y[i] = alt;
    tx += INTERVAL;
    }

  mathutil_get_positive_axis_crossings (x, y, npoints, d_events,
    max, count);
  // Axis crossing times are in seconds after the first x value, that
  //  is, seconds after the 'start' value

  for (int i = 0; i < *count; i++)
    rises[i] = start + d_events[i];

  free (d_events);
  free (x);
  free (y);

  KLOG_OUT
  }

/*============================================================================
  
  moontimes_get_moonsets

  ==========================================================================*/
void moontimes_get_moonsets (time_t start, time_t end, double latitude, 
      double longitude, time_t *rises, int max, int *count) 
  {
  KLOG_IN
  assert (end > start);
  int diff = end - start;
  int npoints = diff / INTERVAL + 1;
  *count = 0;

  double *x = (double *) malloc (npoints * sizeof (double));
  double *y = (double *) malloc (npoints * sizeof (double));
  double *d_events = malloc (max * sizeof (double));

  time_t tx = start;
  for (int i = 0; i < npoints; i++)
    {
    double alt = moonephemera_get_sin_altitude
      (latitude, longitude, tx);
    x[i] = i * INTERVAL;
    y[i] = alt;
    tx += INTERVAL;
    }

  mathutil_get_negative_axis_crossings (x, y, npoints, d_events,
    max, count);
  // Axis crossing times are in seconds after the first x value, that
  //  is, seconds after the 'start' value

  for (int i = 0; i < *count; i++)
    rises[i] = start + d_events[i];

  free (d_events);
  free (x);
  free (y);

  KLOG_OUT
  }


