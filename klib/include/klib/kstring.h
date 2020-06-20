/*============================================================================
  
  klib
  
  knvp.h

  Definition of the KString class

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/types.h>

// Largest number of byte that will be required to store a UTF8
//  code point. This does not include a terminating zero, if required.
// Although UTF8 characters may be represented using more than 4
// bytes, no method in this class will use more than 4.
#define UTF8_MAX_BYTES 4

struct KString;
typedef struct _KString KString;

extern KString *kstring_new_from_utf8 (const UTF8 *utf8);
extern KString *kstring_new_from_utf32 (const UTF32 *utf32);
extern KString *kstring_new_empty (void);
extern void     kstring_destroy (KString *self);

/** A function that can be passed to klist_sort, to sort a list of KString
    objects into alphabetic order */
int kstring_alpha_sort_fn (const void *p1, const void*p2, 
                     void *user_data);

extern void          kstring_append (KString *self, const KString *s);
extern void          kstring_append_char (KString *self, UTF32 c);
extern void          kstring_append_utf32 (KString *self, const UTF32 *s);
extern void          kstring_append_utf8 (KString *self, const UTF8 *s);
extern void          kstring_append_printf (KString *self, char *fmt,...);

extern const UTF32  *kstring_cstr (const KString *self);

/** Returns the numerical value of a specific character. '0'->0, '5'->5, 
    etc. To be able to use this in number base conversion, 'a' and 'A' -> 10,
    and so on. This function will not work with non-ASCII characters. */
extern int           kstring_char_to_number (char c);
extern void          kstring_clear (KString *self);

extern void          kstring_delete (KString *self, int start, int count);

extern BOOL          kstring_ends_with (const KString *self, const KString *s); 
extern BOOL          kstring_ends_with_utf8 (const KString *self, 
                        const  UTF8 *s);
extern BOOL          kstring_ends_with_utf32 (const KString *self, 
                        const  UTF32 *s);
extern size_t        kstring_find (const KString *self, const KString *s);
extern size_t        kstring_find_utf8 (const KString *self, const UTF8 *s);
extern size_t        kstring_find_utf32 (const KString *self, const UTF32 *s);

extern size_t        kstring_find_last (const KString *self, const KString *s);
extern size_t        kstring_find_last_utf8 (const KString *self, 
                        const UTF8 *s);
extern size_t        kstring_find_last_utf32 (const KString *self, 
                        const UTF32 *s);

extern UTF32         kstring_get (const KString *self, size_t i);

// Get a specific character in the string as a set of UTF8 bytes. The
//  buffer passed must be large enough to hold UTF8_MAX_BYTES bytes. The
//  string is NOT null-terminated. The method returns the number of 
//  bytes stored.
extern size_t        kstring_get_utf8 (const KString *self, size_t i, UTF8 *u);
extern size_t        kstring_length (const KString *self);
extern size_t        kstring_length_utf32 (const UTF32 *s);
extern int           kstring_strcmp (const KString *s1, const KString *s2);
extern int           kstring_strcmp_utf8 (const KString *s1, const UTF8 *s2);
extern int           kstring_strcmp_utf32 (const UTF32 *s1, const UTF32 *s2);
extern KString      *kstring_strdup (const KString *self);
/** Make a new string from characters starting at start, with length count.
    It is not an error for count to exceed the length of the string, nor
    for count to equal zero. In both cases the new string takes characters
    up to the end of this one. */ 
extern KString      *kstring_substring (const KString *self, int start, 
                       int count);

/** Convert the string to an integer in the specified radix (base). Returns
    TRUE if the number can be converted. The number can be preceded by 
    '+' or '-' */
extern BOOL          kstring_to_integer (const KString *self, int *value, 
                       int radix);

extern UTF8         *kstring_to_utf8 (const KString *self);

/** Remove whitspace from the start of a string. */
extern void          kstring_trim_left (KString *self);
/** Remove whitspace from the end of a string. */
extern void          kstring_trim_right (KString *self);




