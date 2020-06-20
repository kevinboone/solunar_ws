/*============================================================================
  
  klib
  
  kprops.h

  Definition of the KProps class

  A properties object is a set of name-value pairs, that can be
  populated from a text file. These values can be interpreted as
  strings or numbers. Note that the "utf8" that appears in some
  method names refers not to the format in which data is returned, but
  to the format of the key name supplied to the method.

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/types.h>
#include <klib/defs.h>
#include <klib/klist.h>
#include <klib/kpath.h>
#include <klib/kstring.h>

struct KProps;
typedef struct _KProps KProps;

BEGIN_DECLS

extern KProps *kprops_new_empty (void);
extern void    kprops_destroy (KProps *self);

/** Add a new name-value pair to the properties. If the property already
    exists, it is replaced. */
extern void           kprops_add (KProps *self, const KString *name, 
                         const KString *value);

extern void           kprops_add_utf8 (KProps *self, const UTF8 *name, 
                         const KString *value);

extern BOOL           kprops_from_file (KProps *self, const KPath *path);

extern size_t         kprops_length (const KProps *self);

extern void           kprops_put_boolean (KProps *self, 
                        const KString *key, BOOL value);
extern void           kprops_put_boolean_utf8 (KProps *self, const UTF8 *key, 
                        BOOL value);
extern void           kprops_put_integer (KProps *self, 
                        const KString *key, int value);
extern void           kprops_put_integer_utf8 (KProps *self, 
                        const UTF8 *key, int value);

extern void           kprops_remove (KProps *self, const KString *name);

/** kprops_get and related methods return a reference to an internal
    KString object. The caller should not free or modify it. These
    methods return NULL if the name is not found. */
extern const KString *kprops_get (const KProps *self, const KString *name);
extern const KString *kprops_get_utf8 (const KProps *self, const UTF8 *name);

/** get_boolean returns a 1/0 value. It accepts the strings 'true', 'false',
    'yes', 'no' as well. */
extern  BOOL          kprops_get_boolean (const KProps *self, 
                         const KString *name, BOOL deflt);
extern  BOOL          kprops_get_boolean_utf8 (const KProps *self, 
                        const UTF8 *name, BOOL deflt);

/** get_integer returns a (base-10) integer value. It may be preceded
    by a + or - sign. */
extern int            kprops_get_integer (const KProps *self, 
                         const KString *name, int deflt);
extern int            kprops_get_integer_utf8 (const KProps *self, 
                         const UTF8 *name, int deflt);

END_DECLS
