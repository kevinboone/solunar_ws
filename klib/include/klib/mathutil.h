/*============================================================================
  
  klib
  
  mathutil.h

  Various math utilities

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/defs.h>
#include <klib/types.h>


BEGIN_DECLS

extern double mathutil_asin_deg (double angle); 
extern double mathutil_acos_deg (double angle);
extern double mathutil_cos_deg (double angle);

/** Use Muller's method to interpolate the downward crossing points
 * of the x axis in a set of x-y data points. npoints in the number
 * of points in each x and y array. mins is an array of doubles for
 * the results. maxmins is the number of values that can be stored
 * in mins. nmins is the number of crossing actually found.
 *
 * Incidentally, 'mins' is a bad name, since the results might not
 * necessarily correspond to minima of anything. They are just
 * axis-crossings. */ 
extern void mathutil_get_negative_axis_crossings (double *x, double *y, 
          int npoints, double *mins, int maxmins, int *nmins);

/** As get_negative_axis_crossings, but for upward crossings. */
extern void mathutil_get_positive_axis_crossings (double *x, double *y, 
          int npoints, double *mins, int maxmins, int *nmins);

/** Constrain an angle in degrees to lie in the range 0-359.9... */
extern double mathutil_fix_angle (double angle);

extern double mathutil_sin_deg (double angle);
extern double mathutil_tan_deg (double angle); 
extern double mathutil_round_towards_zero (double value);
extern double mathutil_pascal_frac (double value);

END_DECLS
