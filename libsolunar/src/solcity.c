/*============================================================================
  
  klib
  
  solcity.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <klib/klib.h>
#include <libsolunar/solcity.h>

#define KLOG_CLASS "solunar.solcity"

/*============================================================================
  
 SolCity

  ==========================================================================*/
struct _SolCity
  {
  const char *name;
  const char *code;
  double latitude;
  double longitude;
  const char *tz_name;
  };

#include "cityinfo.h"

/*============================================================================
  
  strcmp_case 

  ==========================================================================*/
static void lower (char *s)
  {
  while (*s)
    {
    *s |= 32;
    s++;
    }
  }

/*============================================================================
  
  solcity_find_matching 

  ==========================================================================*/
KList *solcity_find_matching (const UTF8 *s)
  {
  KLOG_IN
  assert (s != NULL);
  KList *list = NULL;
  klog_debug (KLOG_CLASS, "Find city matching '%s'", s);

  char *slwr = strdup ((char *)s);
  lower (slwr);
  int i = 0;
  SolCity *c = &cities[i];
  while (c->name)
    {
    char *name = strdup (c->name);
    lower (name);
    if (strstr (name, (char *)slwr)) 
      {
      if (!list) list = klist_new_empty ((KListFreeFn)NULL);
      // Add to list
      klist_append (list, c);
      klog_debug (KLOG_CLASS, "Found '%s'", c->name);
      }
    i++;
    c = &cities[i];
    free (name);
    }

  free (slwr);
  KLOG_OUT
  return list;
  }

/*============================================================================
  
  solcity_get_latitude 

  ==========================================================================*/
double solcity_get_latitude (const SolCity *self)
  {
  KLOG_IN
  double ret = self->latitude;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  solcity_get_longitude

  ==========================================================================*/
double solcity_get_longitude (const SolCity *self)
  {
  KLOG_IN
  double ret = self->longitude;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  solcity_get_name

  ==========================================================================*/
const char *solcity_get_name (const SolCity *self)
  {
  KLOG_IN
  const char *ret = self->name; 
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  solcity_get_tz_name

  ==========================================================================*/
const char *solcity_get_tz_name (const SolCity *self)
  {
  KLOG_IN
  const char *ret = self->tz_name; 
  KLOG_OUT
  return ret;
  }


