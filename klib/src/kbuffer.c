/*============================================================================
  
  klib
  
  kbuffer.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <klib/klog.h>
#include <klib/knvp.h>
#include <klib/kbuffer.h>
#include <klib/kstring.h>

#define KLOG_CLASS "klib.kbuffer"

/*============================================================================
  
  KBuffer

  ==========================================================================*/
struct _KBuffer
  {
  BYTE *data;
  int length;
  };


/*============================================================================
  
  kbuffer_new_empty

  ==========================================================================*/
KBuffer *kbuffer_new_empty (void)
  {
  KLOG_IN
  KBuffer *self = malloc (sizeof (KBuffer));
  self->length = 0;
  self->data = NULL;
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  kbuffer_new_from_data

  ==========================================================================*/
KBuffer *kbuffer_new_from_data (const BYTE *b, int64_t size)
  {
  KLOG_IN
  
  klog_debug (KLOG_CLASS, "Creating buffer of size %ld", size);
  KBuffer *self = NULL;

  BYTE *data = malloc (size * sizeof (int64_t));
  if (data)
    {
    self = malloc (sizeof (KBuffer));
    self->length = size;
    self->data = data;
    memcpy (data, b, size);
    for (int i = 0; i < sizeof (int32_t); i++)
      data [size + i] = 0;
    }
  else
    {
    klog_error (KLOG_CLASS, "malloc(%ld) failed", size);
    errno = ENOMEM;
    }
 
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  kbuffer_new_from_data_no_copy

  ==========================================================================*/
extern KBuffer *kbuffer_new_from_data_no_copy (BYTE *b, int64_t size)
  {
  KLOG_IN
  KBuffer *self = malloc (sizeof (KBuffer));
  self->length = size;
  self->data = b;
  KLOG_OUT
  return self;
  }
  
/*============================================================================
  
  kbuffer_destroy

  ==========================================================================*/
void kbuffer_destroy (KBuffer *self)
  {
  KLOG_IN
  if (self)
    {
    if (self->data) free (self->data);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
  
  kbuffer_get_data

  ==========================================================================*/
BYTE *kbuffer_get_data (const KBuffer *self)
  {
  KLOG_IN
  assert (self != NULL);
  BYTE *ret = self->data;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kbuffer_get_size

  ==========================================================================*/
int64_t kbuffer_get_size (const KBuffer *self)
  {
  KLOG_IN
  assert (self != NULL);
  int64_t ret = self->length;
  KLOG_OUT
  return ret;
  }

