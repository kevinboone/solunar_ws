/*============================================================================
  
  klib
  
  knvp.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#include <stdlib.h>
#include <assert.h>
#include <klib/knvp.h>
#include <klib/kstring.h>
#include <klib/klog.h>

#define KLOG_CLASS "klib.knvp"

/*============================================================================
  
  KNVP

  ==========================================================================*/
struct _KNVP
  {
  KString *name;
  KString *value;
  };

/*============================================================================
  
  knvp_new

  ==========================================================================*/
KNVP *knvp_new (const KString *name, const KString *value)
  {
  KLOG_IN
  KNVP *self = malloc (sizeof (KNVP));
  self->name = kstring_strdup (name);
  self->value = kstring_strdup (value);
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  knvp_new_from_utf8

  ==========================================================================*/
KNVP *knvp_new_from_utf8 (const UTF8 *name, const UTF8 *value)
  {
  KLOG_IN
  KNVP *self = malloc (sizeof (KNVP));
  self->name = kstring_new_from_utf8 (name);
  self->value = kstring_new_from_utf8 (value);
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  knvp_new_from_utf8

  ==========================================================================*/
void knvp_destroy (KNVP *self)
  {
  KLOG_IN
  if (self)
    {
    kstring_destroy (self->name);
    kstring_destroy (self->value);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
  
  knvp_get_name

  ==========================================================================*/
extern KString *knvp_get_name (const KNVP *self)
  {
  KLOG_IN
  assert (self != NULL);
  KString *ret = self->name;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  knvp_get_value

  ==========================================================================*/
extern KString *knvp_get_value (const KNVP *self)
  {
  KLOG_IN
  assert (self != NULL);
  KString *ret = self->value;
  KLOG_OUT
  return ret;
  }


