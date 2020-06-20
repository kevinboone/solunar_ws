/*============================================================================
  
  klib
  
  kbffer.h

  Definition of the KBuffer class

  A KBuffer is a block of arbitrary data, like malloc() with a stored
  size. In general, when data is stored in a KBuffer, some additional
  zeros are added, to make it easier to dump the contents for debugging
  purposes. These zeros are not counted in the total size.

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/types.h>
#include <klib/defs.h>

struct _KBuffer;
typedef struct _KBuffer KBuffer;

BEGIN_DECLS

extern KBuffer  *kbuffer_new_empty (void);

/** Create a buffer from the specified data, which is copied. The caller
     can -- and probably should -- free its own version.*/
extern KBuffer  *kbuffer_new_from_data (const BYTE *b, int64_t size);

/** Create a buffer from the specified dat, which then belongs to the 
    buffer. The caller should not modify or free it. */
extern  KBuffer *kbuffer_new_from_data_no_copy (BYTE *b, int64_t size);

extern void      kbuffer_destroy (KBuffer *self);

extern int64_t  kbuffer_get_size (const KBuffer *self);
extern BYTE    *kbuffer_get_data (const KBuffer *self);

END_DECLS


