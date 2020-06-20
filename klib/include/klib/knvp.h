/*============================================================================
  
  klib
  
  knvp.h

  Definition of the KNVP class

  This class holds a name-value pair

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/types.h>
#include <klib/kstring.h>

struct KNVP;
typedef struct _KNVP KNVP;

extern KNVP *knvp_new (const KString *name, const KString *value);
extern KNVP *knvp_new_from_utf8 (const UTF8 *name, const UTF8 *value);
extern void  knvp_destroy (KNVP *self);

extern KString *knvp_get_name (const KNVP *self);
extern KString *knvp_get_value (const KNVP *self);

