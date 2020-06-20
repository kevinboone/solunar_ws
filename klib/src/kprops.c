/*============================================================================
  
  klib
  
  kprops.c

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
#include <klib/klist.h>
#include <klib/kprops.h>
#include <klib/knvp.h>
#include <klib/kpath.h>
#include <klib/kstring.h>

#define KLOG_CLASS "klib.kprops"

/*============================================================================
  
  KProps

  ==========================================================================*/
struct _KProps
  {
  KList *list;
  };


/*============================================================================
  
  kprops_new_empty

  ==========================================================================*/
KProps *kprops_new_empty (void)
  {
  KLOG_IN
  KProps *self = malloc (sizeof (KProps));
  self->list = klist_new_empty ((KListFreeFn)knvp_destroy);
  KLOG_OUT
  return self;
  }
  
/*============================================================================
  
  kprops_destroy

  ==========================================================================*/
void kprops_destroy (KProps *self)
  {
  KLOG_IN
  if (self)
    {
    assert (self->list != NULL);
    klist_destroy (self->list);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
  
  kprops_add

  ==========================================================================*/
void kprops_add (KProps *self, const KString *name, const KString *value)
  {
  KLOG_IN
  klog_debug (KLOG_CLASS, "%s: add prop %S=%S", __PRETTY_FUNCTION__,  
    kstring_cstr(name), kstring_cstr(value));
  kprops_remove (self, name);
  KNVP *knvp = knvp_new (name, value);
  klist_append (self->list, knvp);
  KLOG_OUT
  }

/*============================================================================
  
  kprops_add_utf8

  ==========================================================================*/
void kprops_add_utf8 (KProps *self, const UTF8 *name, const KString *value)
  {
  KLOG_IN
  klog_debug (KLOG_CLASS, "%s: add prop %s=%S", __PRETTY_FUNCTION__,  
    name, kstring_cstr(value));
  KString *temp = kstring_new_from_utf8 (name);
  kprops_remove (self, temp);
  KNVP *knvp = knvp_new (temp, value);
  klist_append (self->list, knvp);
  kstring_destroy (temp);
  KLOG_OUT
  }

/*============================================================================
  
  kprops_from_file

  ==========================================================================*/
BOOL kprops_from_file (KProps *self, const KPath *path)
  {
  KLOG_IN
  BOOL ret = FALSE;
  assert (self != NULL);
  assert (path != NULL);
  klog_debug (KLOG_CLASS, "Reading properties from file '%S'", 
    kstring_cstr ((KString *)path));
  
  FILE *f = kpath_fopen (path, "r");
  if (f)
    {
    klog_debug (KLOG_CLASS, "File opened OK");

    char *line = NULL;
    size_t n = 0;
    while (getline (&line, &n, f) >= 0)
      {
      KString *line2 = kstring_new_from_utf8 ((UTF8*)line);
      kstring_trim_left (line2);
      kstring_trim_right (line2);
      if (kstring_length (line2) > 1)
        {
        if (kstring_get (line2, 0) != '#')
          {
          size_t eq = kstring_find_utf8 (line2, (UTF8 *)"=");
          if (eq > 0)
            {
            KString *name = kstring_substring (line2, 0, eq); 
            KString *value = kstring_substring (line2, eq + 1, 0); 
            klog_debug (KLOG_CLASS, "Read prop from file: key=%S val=%S", 
               kstring_cstr(name), kstring_cstr(value));
            kprops_add (self, name, value);
            kstring_destroy (name);
            kstring_destroy (value);
            }
          }
        }
      kstring_destroy (line2);
      free (line);
      line = NULL;
      n = 0;
      }
    ret = TRUE;
    fclose (f);
    if (line) free (line);
    }
  else
    {
    klog_debug (KLOG_CLASS, "Can't open file '%S' for reading: %s", 
      kstring_cstr ((KString *)path), strerror (errno));
    }

  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kprops_get

  ==========================================================================*/
const KString *kprops_get (const KProps *self, const KString *name)
  {
  KLOG_IN
  assert (self != NULL);
  assert (name != NULL);
  klog_debug (KLOG_CLASS, "kprops_get, name=%S", kstring_cstr(name));
  KString *ret = NULL;
  size_t l = klist_length (self->list);
  for (int i = 0; i < l && !ret; i++)
    {
    const KNVP *nvp = klist_get (self->list, i);
    if (nvp)
      {
      const KString *aname = knvp_get_name (nvp);
      if (kstring_strcmp (name, aname) == 0)
        {
        klog_debug (KLOG_CLASS, "found prop");
        ret = knvp_get_value (nvp);
        }
      }
    }
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kprops_get_utf8

  ==========================================================================*/
const KString *kprops_get_utf8 (const KProps *self, const UTF8 *name)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (name);
  const KString *ret = kprops_get (self, temp);
  kstring_destroy (temp);
  KLOG_OUT
  return ret; 
  }

/*============================================================================
  
  kprops_get_boolean

  ==========================================================================*/
BOOL kprops_get_boolean (const KProps *self, const KString *name, BOOL deflt)
  {
  KLOG_IN
  int ret = deflt;
  const KString *v = kprops_get (self, name);
  if (v)
    {
    KString *v2 = kstring_strdup (v);
    kstring_trim_left (v2);
    kstring_trim_right (v2);
    if (kstring_strcmp_utf8 (v2, (UTF8*)"true") == 0)
      ret = TRUE;
    else if (kstring_strcmp_utf8 (v2, (UTF8*)"false") == 0)
      ret = FALSE;
    else if (kstring_strcmp_utf8 (v2, (UTF8*)"0") == 0)
      ret = FALSE;
    else if (kstring_strcmp_utf8 (v2, (UTF8*)"1") == 0)
      ret = TRUE;
    else if (kstring_strcmp_utf8 (v2, (UTF8*)"no") == 0)
      ret = FALSE;
    else if (kstring_strcmp_utf8 (v2, (UTF8*)"yes") == 0)
      ret = TRUE;
    kstring_destroy (v2); 
    }
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kprops_get_boolean_utf8

  ==========================================================================*/
BOOL kprops_get_boolean_utf8 (const KProps *self, const UTF8 *name, BOOL deflt)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (name);
  BOOL ret = kprops_get_boolean (self, temp, deflt);
  kstring_destroy (temp);
  KLOG_OUT
  return ret; 
  }

/*============================================================================
  
  kprops_get_integer

  ==========================================================================*/
int kprops_get_integer (const KProps *self, const KString *name, int deflt)
  {
  KLOG_IN
  int ret = deflt;
  const KString *v = kprops_get (self, name);
  if (v)
    {
    int i;
    if (kstring_to_integer (v, &i, 10))
      ret = i;
    }
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kprops_get_integer_utf8

  ==========================================================================*/
int kprops_get_integer_utf8 (const KProps *self, const UTF8 *name, int deflt)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (name);
  int ret = kprops_get_integer (self, temp, deflt);
  kstring_destroy (temp);
  KLOG_OUT
  return ret; 
  }

/*============================================================================
  
  kprops_length

  ==========================================================================*/
size_t kprops_length (const KProps *self)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->list != NULL);
  size_t ret = klist_length (self->list);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kprops_put_boolean

  ==========================================================================*/
void kprops_put_boolean (KProps *self, const KString *key, BOOL value)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->list != NULL);
  KString *s = kstring_new_empty();
  kstring_append_printf (s, "%d", value);
  kprops_add (self, key, s);
  kstring_destroy (s);
  KLOG_OUT
  }

/*============================================================================
  
  kprops_put_boolean_utf8

  ==========================================================================*/
void kprops_put_boolean_utf8 (KProps *self, const UTF8 *key, BOOL value)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (key);
  kprops_put_boolean (self, temp, value);
  kstring_destroy (temp);
  KLOG_OUT
  }

/*============================================================================
  
  kprops_put_integer

  ==========================================================================*/
extern void kprops_put_integer (KProps *self, 
                        const KString *key, int value)
  {
  assert (self != NULL);
  assert (self->list != NULL);
  KString *s = kstring_new_empty();
  kstring_append_printf (s, "%d", value);
  kprops_add (self, key, s);
  kstring_destroy (s);
  }

/*============================================================================
  
  kprops_put_integer_utf8

  ==========================================================================*/
extern void kprops_put_integer_utf8 (KProps *self, 
                        const UTF8 *key, int value)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (key);
  kprops_put_integer (self, temp, value);
  kstring_destroy (temp);
  KLOG_OUT
  }

/*============================================================================
  
  kprops_remove

  ==========================================================================*/
void kprops_remove (KProps *self, const KString *name)
  {
  KLOG_IN
  assert (self != NULL);

  klog_debug (KLOG_CLASS, "remove props, key=%S", 
      kstring_cstr(name));
  
  BOOL found = FALSE;
  size_t l = klist_length (self->list);
  for (int i = 0; i < l && !found; i++)
    {
    const KNVP *nvp = klist_get (self->list, i);
    if (nvp)
      {
      const KString *aname = knvp_get_name (nvp);
      if (kstring_strcmp (name, aname) == 0)
        {
        klog_debug (KLOG_CLASS, "kprops_remove, found NVP, deleting");
        klist_remove_ref (self->list, nvp, TRUE);
        found = TRUE;
        }
      }
    }

  assert (self->list != NULL);
  KLOG_OUT
  }


