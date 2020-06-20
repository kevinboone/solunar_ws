/*==========================================================================

  klib

  mathutil.c

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
#include <klib/mathutil.h> 

#define KLOG_CLASS "klib.mathutil"

static const double TWO_PI = 2.0 * M_PI; 

/*============================================================================
  
  mathutil_acos_deg
  Arc cosine of an angle in degrees

  ==========================================================================*/
double mathutil_acos_deg (double angle)
  {
  return acos (angle) / TWO_PI * 360.0;
  }

/*============================================================================
  
  mathutil_asin_deg
  Arc sine of an angle in degrees

  ==========================================================================*/
double mathutil_asin_deg (double angle)
  {
  return asin (angle) / TWO_PI * 360.0;
  }

/*============================================================================
  
  mathutil_cos_deg
  Sine of an angle in degrees

  ==========================================================================*/
double mathutil_cos_deg (double angle)
  {
  return cos (angle / 360.0 * TWO_PI);
  }

/*============================================================================
  
  mathutil_fix_angle

  ==========================================================================*/
double mathutil_fix_angle (double angle)
  {
  KLOG_IN
  double result = angle - 360.0 * (floor(angle / 360.0));
  KLOG_OUT
  return result;
  }

/*============================================================================
  
  mathutil_get_negative_axis_crossings

  ==========================================================================*/
void mathutil_get_negative_axis_crossings (double *x, double *y, 
  int npoints, double *mins, int maxmins, int *nmins)
  {
  KLOG_IN
  *nmins = 0;

  int i;
  for (i = 1; i < npoints - 1  && *nmins < maxmins; i++)
    {
    double y0 = y[i - 1];
    double y1 = y[i];
    double y2 = y[i + 1];
    if (y0 > 0 && y1 >= 0 && y2 < 0)
      {
      double root1 = 0.0;
      double root2 = 0.0;
      double A = (0.5 * (y0 + y2)) - y1;
      double B = (0.5 * (y2 - y0));
      double C = y1;
      double xExtreme = -B / (2.0 * A);
      double discriminant = (B * B) - 4.0 * A * C;
      if (discriminant >= 0.0)
        { 
        double DX = 0.5 * sqrt(discriminant) / fabs(A);
        root1 = xExtreme - DX;
        root2 = xExtreme + DX;
        double bisect = 0;
        if (fabs (root2) < 1.0)
          bisect = root2;
        else if (fabs (root1) < 1.0)
          bisect = root1;
        double xguess = x[i] + (bisect * (x[i] - x[i-1])); 
        mins[*nmins] = xguess;
        (*nmins)++;
        }
      }
    } 
  KLOG_OUT
  }

/*============================================================================
  
  mathutil_get_positive_axis_crossings

  ==========================================================================*/
void mathutil_get_positive_axis_crossings (double *x, double *y, 
        int npoints, double *mins, int maxmins, int *nmins)
  {
  KLOG_IN
  *nmins = 0;

  int i;
  for (i = 1; i < npoints - 1  && *nmins < maxmins; i++)
    {
    double y0 = y[i - 1];
    double y1 = y[i];
    double y2 = y[i + 1];
    if (y0 < 0 && y1 <= 0 && y2 > 0)
      {
      double root1 = 0.0;
      double root2 = 0.0;
      double A = (0.5 * (y0 + y2)) - y1;
      double B = (0.5 * (y2 - y0));
      double C = y1;
      double xExtreme = -B / (2.0 * A);
      double discriminant = (B * B) - 4.0 * A * C;
      if (discriminant >= 0.0)
        { 
        double DX = 0.5 * sqrt(discriminant) / fabs(A);
        root1 = xExtreme - DX;
        root2 = xExtreme + DX;
        double bisect = 0;
        if (fabs (root2) < 1.0)
          bisect = root2;
        else if (fabs (root1) < 1.0)
          bisect = root1;
        double xguess = x[i] + (bisect * (x[i] - x[i-1])); 
        mins[*nmins] = xguess;
        (*nmins)++;
        }
      }
    } 
  KLOG_OUT
  }

/*============================================================================
  
  mathutil_pascal_frac

  ==========================================================================*/
double mathutil_pascal_frac (double value)
  {
  KLOG_IN
  double result = value - mathutil_round_towards_zero (value);
  if (result < 0.0)
    result += 1.0;
  KLOG_OUT
  return (result);
  }

/*============================================================================
  
  mathutil_round_towards_zero

  ==========================================================================*/
double mathutil_round_towards_zero (double value)
  {
  KLOG_IN
  double result = floor (fabs (value));
  if (value < 0.0)
    result = -result;
  KLOG_OUT
  return result;
  }

/*============================================================================
  
  mathutil_sin_deg
  Sine of an angle in degrees

  ==========================================================================*/
double mathutil_sin_deg (double angle)
  {
  return sin (angle / 360.0 * TWO_PI);
  }

/*============================================================================
  
  mathutil_tan_deg
  Sine of an angle in degrees

  ==========================================================================*/
double mathutil_tan_deg (double angle)
  {
  return tan (angle / 360.0 * TWO_PI);
  }

