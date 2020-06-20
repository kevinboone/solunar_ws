/*============================================================================
  
  klib
  
  kstring.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <stdarg.h>
#include "convertutf.h" 
#include <klib/kstring.h>
#include <klib/klog.h>

#define KLOG_CLASS "klib.kstring"

/*============================================================================
  
  KString 

  ==========================================================================*/
struct _KString
  {
  size_t length;
  UTF32 *str;
  };


/*============================================================================
  
  KString *kstring_new_empty

  ==========================================================================*/
KString *kstring_new_empty (void)
  {
  KLOG_IN
  KString *self = malloc (sizeof (KString));
  self->str = malloc(1 * sizeof (UTF32));
  self->str[0] = 0;
  self->length = 0;
  KLOG_OUT
  return self;
  }


/*============================================================================
  
  KString *kstring_new_from_utf8

  ==========================================================================*/
KString *kstring_new_from_utf8 (const UTF8 *_in)
  {
  KLOG_IN
  assert (_in != NULL);
  KString *self = malloc (sizeof (KString));

  const UTF8* in = (UTF8 *)_in;
  int max_out = strlen ((char *)_in); // This is an absolute maximum
  UTF32 *out = malloc ((max_out + 1) * sizeof (UTF32));
  memset (out, 0, max_out * sizeof (UTF32));
  UTF32 *out_temp = out;

  ConvertUTF8toUTF32 ((const UTF8 **)&in, (const UTF8 *)in+strlen((char *)in),
      &out_temp, out + max_out, 0);

  int len = out_temp - out;
  out [len] = 0;
  self->str = out;
  self->length = len;
  KLOG_OUT
  return self;
  }


/*============================================================================
  
  KString *kstring_new_from_utf32

  ==========================================================================*/
KString *kstring_new_from_utf32 (const UTF32 *s)
  {
  KLOG_IN
  assert (s != NULL);
  KString *self = malloc (sizeof (KString));
  self->length = kstring_length_utf32 (s);
  self->str = malloc ((self->length + 1) * sizeof (UTF32));
  memcpy (self->str, s, (self->length + 1) * sizeof (UTF32));
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  kstring_destroy 

  ==========================================================================*/
void kstring_destroy (KString *self)
  {
  KLOG_IN
  if (self)
    {
    if (self->str) free (self->str);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
  
  kstring_alpha_sort_fn

  ==========================================================================*/
int kstring_alpha_sort_fn (const void *p1, const void *p2, void *user_data)
  {
  KLOG_IN
  assert (p1 != NULL);
  assert (p2 != NULL);
  KString *ss1 = * (KString *const *) p1;
  KString *ss2 = * (KString *const *) p2;
  int ret = kstring_strcmp (ss1, ss2);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_append

  ==========================================================================*/
void kstring_append (KString *self, const KString *s)
  {
  KLOG_IN
  assert (self != NULL);
  assert (s != NULL);
  int newlen = self->length + s->length;
  self->str = realloc (self->str, (newlen + 1) * sizeof (UTF32));
  memcpy (self->str + self->length, s->str, (s->length * sizeof (UTF32)));
  self->length = newlen;
  self->str [self->length] = 0;
  KLOG_OUT
  }

/*============================================================================
  
  kstring_append_printf

  Better hope the C library memory allocator is efficient, because this
    implementation is not :/

  ==========================================================================*/
void kstring_append_char (KString *self, UTF32 c)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->str != NULL);

  self->str = realloc (self->str, (self->length + 2) * sizeof (UTF32)); 
  self->str[self->length] = c;
  self->str[self->length + 1] = 0;
  self->length += 1;

  KLOG_OUT
  }

/*============================================================================
  
  kstring_append_printf

  ==========================================================================*/
void kstring_append_printf (KString *self, char *fmt,...)
  {
  KLOG_IN
  assert (self != NULL);
  assert (fmt != NULL);
  va_list ap;
  va_start (ap, fmt);
  char *s;
  vasprintf (&s, fmt, ap);
  kstring_append_utf8 (self, (UTF8*)s);
  free (s);
  va_end (ap);
  KLOG_OUT
  }

/*============================================================================
  
  kstring_append_utf8

  ==========================================================================*/
void kstring_append_utf8 (KString *self, const UTF8 *s)
  {
  KLOG_IN
  assert (s != NULL);
  KString *temp = kstring_new_from_utf8 (s);
  kstring_append (self, temp);
  kstring_destroy (temp);
  KLOG_OUT
  }

/*============================================================================
  
  kstring_append_utf32

  ==========================================================================*/
void kstring_append_utf32 (KString *self, const UTF32 *s)
  {
  KLOG_IN
  kstring_append_utf32 (self, s);
  KLOG_OUT
  }

/*============================================================================
  
  kstring_cstr

  ==========================================================================*/
const UTF32 *kstring_cstr (const KString *self)
  {
  KLOG_IN
  assert (self != NULL);
  UTF32 *ret = self->str;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_clear

  ==========================================================================*/
void kstring_clear (KString *self) 
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->str != NULL);
  free (self->str);
  self->str = malloc (sizeof (UTF32));
  self->str[0] = 0;
  self->length = 0;
  KLOG_OUT
  }

/*============================================================================
  
  kstring_delete

  ==========================================================================*/
void kstring_delete (KString *self, int pos, int len)
  {
  KLOG_IN
  UTF32 *str = self->str;
  int lself = self->length; 
  if (pos + len > lself)
    kstring_delete (self, pos, lself - len);
  else
    {
    UTF32 *buff = malloc ((lself - len + 2) * sizeof (UTF32));
    memcpy (buff, str, pos  * sizeof (UTF32));
    memcpy (buff + pos, str + pos + len,
      (1 + kstring_length_utf32 (str + pos + len)) * sizeof (UTF32));
    free (self->str);
    self->str = buff;
    self->length -= len;
    }
  KLOG_OUT 
  }

/*============================================================================
  
  kstring_ends_with

  ==========================================================================*/
BOOL kstring_ends_with (const KString *self, 
                        const  KString *s)
  {
  KLOG_IN
  BOOL ret = kstring_ends_with_utf32 (self, s->str); 
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kstring_ends_with_utf8

  ==========================================================================*/
BOOL kstring_ends_with_utf8 (const KString *self, 
                        const UTF8 *s)
  {
  KLOG_IN
  assert (self != NULL);
  assert (s!= NULL);
  KString *temp = kstring_new_from_utf8 (s);
  BOOL ret = kstring_ends_with (self, temp);
  kstring_destroy (temp);
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kstring_ends_with_utf32

  ==========================================================================*/
BOOL kstring_ends_with_utf32 (const KString *self, 
                        const  UTF32 *s)
  {
  KLOG_IN
  BOOL ret = TRUE;
  assert (self != NULL);
  assert (s!= NULL);
  int l = self->length;
  int l2 = kstring_length_utf32 (s);
  if (l2 <= l)
    {
    for (int i = 0; i < l2 && ret; i++)
      {
      if (self->str[i + (l - l2)] != s[i]) ret = FALSE;
      }
    }
  else
    ret = FALSE;
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_find

  ==========================================================================*/
size_t kstring_find (const KString *self, const KString *s)
  {
  KLOG_IN
  size_t ret = kstring_find_utf32 (self, s->str);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_find_utf8

  ==========================================================================*/
size_t kstring_find_utf8 (const KString *self, const UTF8 *s)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (s);
  size_t ret = kstring_find (self, temp);
  kstring_destroy (temp);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_find_utf32

  ==========================================================================*/
size_t kstring_find_utf32 (const KString *self, const UTF32 *s)
  {
  KLOG_IN
  size_t ret = -1;
  int lsearch = kstring_length_utf32 (s);
  int lself = self->length; 
  if (lsearch > lself) return -1; // Can't find a long string in short one
  for (int i = 0; i < lself - lsearch + 1 && ret == -1; i++)
    {
    BOOL diff = FALSE;
    for (int j = 0; j < lsearch && !diff; j++)
      {
      if (s[j] != self->str[i + j]) diff = TRUE;
      }
    if (!diff) ret = i;
    }

  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_find_last

  ==========================================================================*/
size_t kstring_find_last (const KString *self, const KString *s)
  {
  KLOG_IN
  size_t ret = kstring_find_last_utf32 (self, s->str);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_find_last_utf8

  ==========================================================================*/
size_t kstring_find_last_utf8 (const KString *self, const UTF8 *s)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (s);
  size_t ret = kstring_find_last (self, temp);
  kstring_destroy (temp);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_find_last_utf32

  ==========================================================================*/
size_t kstring_find_last_utf32 (const KString *self, const UTF32 *search)
  {
  int lsearch = kstring_length_utf32 (search);
  int lself = self->length;
  if (lsearch > lself) 
    {
    return -1; // Can't find a long string in short one
    }
  else
    {
    for (int i = lself - lsearch; i >= 0; i--)
      {
      BOOL diff = FALSE;
      for (int j = 0; j < lsearch; j++)
        {
        if (search[j] != self->str[i + j]) diff = TRUE;
        }
      if (!diff) return i;
      }
    }

  return -1;
  }

/*============================================================================
  
  kstring_length

  ==========================================================================*/
size_t kstring_length (const KString *self)
  {
  KLOG_IN
  assert (self != NULL);
  size_t ret = self->length;
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kstring_length_utf32

  ==========================================================================*/
size_t kstring_length_utf32 (const UTF32 *s)
  {
  KLOG_IN
  assert (s != NULL);
  size_t ret = 0;
  while (*s) { s++; ret++; } 
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kstring_get

  ==========================================================================*/
UTF32 kstring_get (const KString *self, size_t i)
  {
  KLOG_IN
  assert (self != NULL);
  assert (i < self->length);
  UTF32 ret = self->str[i];
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_get_utf8

  ==========================================================================*/
size_t kstring_get_utf8 (const KString *self, size_t i, UTF8 *u)
  {
  KLOG_IN
  assert (self != NULL);
  assert (i < self->length);
  int ret = 0;
  int c = self->str[i];

  if (c < 0x80)
    {
    *u = (BYTE)c;
    ret = 1;
    }
  else if (c < 0x0800)
    {
    *u = (BYTE)((c >> 6) | 0xC0); u++;
    *u = (BYTE)((c & 0x3F) | 0x80);
    ret = 2;
    }
   else if (c < 0x10000)
    {
    *u = (BYTE)((c >> 12) | 0xE0); u++;
    *u = (BYTE)((c >> 6 & 0x3F) | 0x80); u++;
    *u = (BYTE)((c & 0x3F) | 0x80);
    ret = 3;
    }
   else
    {
    *u = (BYTE)((c >> 18) | 0xF0); u++;
    *u = (BYTE)(((c >> 12) & 0x3F) | 0x80); u++;
    *u = (BYTE)(((c >> 6) & 0x3F) | 0x80); u++;
    *u = (BYTE)((c & 0x3F) | 0x80);
    ret = 4;
    }


  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kstring_strcmp

  ==========================================================================*/
int kstring_strcmp (const KString *s1, const KString *s2)
  {
  return kstring_strcmp_utf32 (s1->str, s2->str);
  }

/*============================================================================
  
  kstring_strcmp_utf8

  ==========================================================================*/
int kstring_strcmp_utf8 (const KString *s1, const UTF8 *s2)
  {
  KLOG_IN
  KString *temp = kstring_new_from_utf8 (s2);
  int ret = kstring_strcmp (s1, temp); 
  kstring_destroy (temp);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_strcmp_utf32

  ==========================================================================*/
int kstring_strcmp_utf32 (const UTF32 *s1, const UTF32 *s2)
  {
  KLOG_IN
  while (*s1 && (*s1 == *s2))
    {
    ++s1;
    ++s2;
    }
  int ret =  *(UTF32 *)s1 - *(UTF32 *)s2 ;
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kstring_strdup

  ==========================================================================*/
KString *kstring_strdup (const KString *self)
  {
  KLOG_IN
  KString *ret = kstring_new_from_utf32 (self->str);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_to_utf8

  ==========================================================================*/
KString *kstring_substring (const KString *self, int start, int count)
  {
  KLOG_IN
  assert (self != NULL);
  assert (start >= 0);
  if (count == 0)
    count = self->length - start;
  if (count + start >= self->length) 
    count = self->length - start;
  UTF32 *s = malloc ((count + 1) * sizeof (UTF32));
  memcpy (s, self->str + start, count  * sizeof (UTF32));
  s[count] = 0;
  KString *ret = kstring_new_from_utf32 (s);
  free (s);
  KLOG_OUT
  return ret; 
  }

/*============================================================================
  
  kstring_char_to_number

  ==========================================================================*/
int kstring_char_to_number (char c)
  {
  KLOG_IN
  int ret = -1;
  if (c >= '0' && c <= '9') ret = c - '0'; 
  if (c >= 'A' && c <= 'Z') ret = c - 'A' + 10; 
  if (c >= 'a' && c <= 'z') ret = c - 'a' + 10; 
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_to_utf8

  ==========================================================================*/
BOOL kstring_to_integer (const KString *self, int *value, int radix)
  {
  KLOG_IN
  int ret = TRUE;
  if (self->length == 0)
    {
    ret = FALSE;
    }
  else
    {
    BOOL negate = FALSE;
    int base = 1;
    *value = 0;

    int start = 0;
    int first = self->str[0]; 
    if (first == '+')
      {
      start++;
      }
    else if (first == '-')
      {
      negate = TRUE;
      start++;
      }

    if (self->length - start >= 1) 
      {
      for (int i = self->length - 1; i >= start && ret; i--)
	{
	int d = kstring_char_to_number (self->str[i]);
	if (d < 0) 
	  ret = FALSE;
        else if (d >= radix)
          ret = FALSE;
	else
	  {
	  *value += d * base;
	  base *= radix; 
	  }
	}
      if (negate) *value = - (*value);
      }
    else
      ret = FALSE;
    }

  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kstring_to_utf8

  ==========================================================================*/
UTF8 *kstring_to_utf8 (const KString *self)
  {
  KLOG_IN
  assert (self != NULL);
  const UTF32 *in = self->str;

  int max_out = self->length * 5 + 1; // This is an absolute maximum
  UTF8 *out = malloc ((max_out + 1) * sizeof (UTF8));
  memset (out, 0, max_out * sizeof (UTF8));
  UTF8 *out_temp = out;

  ConvertUTF32toUTF8 ((const UTF32 **)&in, (const UTF32 *)in+self->length,
      &out_temp, out + max_out, 0);

  int len = out_temp - out;
  out[len] = 0;

  KLOG_OUT
  return (UTF8 *)out;
  }

/*============================================================================
  
  kstring_trim_left

  ==========================================================================*/
void kstring_trim_left (KString *self)
  {
  KLOG_IN
  const UTF32 *s = self->str;
  int l = self->length; 
  int i = 0;
  int pos = 0;
  BOOL stop = FALSE;
  while (i < l && !stop)
    {
    char c = s[i];
    if (c == ' ' || c == '\n' || c == '\t')
      {
      pos++;
      }
    else
      stop = TRUE;
    i++;
    }

  int new_len = l - pos;
  UTF32 *s_new = malloc ((new_len + 1) * sizeof (UTF32));
  memcpy (s_new, self->str + pos, (new_len + 1) * sizeof (UTF32));
  free (self->str);
  self->str = s_new;
  self->length = new_len; 
  KLOG_OUT
  }

/*============================================================================
  
  kstring_trim_right

  ==========================================================================*/
void kstring_trim_right (KString *self)
  {
  KLOG_IN
  UTF32 *s = self->str;
  int l = self->length; 
  int i = l - 1;
  BOOL stop = FALSE;
  while (i >= 0 && !stop)
    {
    UTF32 c = s[i];
    if (c == ' ' || c == '\n' || c == '\t')
      {
      s[i] = 0;
      }
    else
      stop = TRUE;
    i--;
    }
  self->length = i + 2;
  KLOG_OUT
  }


