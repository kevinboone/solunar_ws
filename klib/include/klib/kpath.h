/*============================================================================
  
  klib
  
  kpath.h

  Definition of the KPath class

  KPath models a pathname. It is based on KString, and all the KString
  methods can be used, with the appropriate casts. Some of these methods
  are wrappers around ordinary C library calls like open() and stat().
  Such calls return a BOOL which is TRUE for success, FALSE otherwise.
  Care has to be taken to avoid using "int" for these return values, because
  C library calls that return an int usually return 0 for success.

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <time.h>
#include <sys/stat.h>
#include <klib/defs.h>
#include <klib/types.h>
#include <klib/kbuffer.h>
#include <klib/kstring.h>
#include <klib/klist.h>

#ifdef WIN32
#define KPATH_SEP_CHAR '\\'
#define KPATH_SEP_UTF8 "\\"
#else
#define KPATH_SEP_CHAR '/'
#define KPATH_SEP_UTF8 "/"
#endif

// Constants for controlling kpath_expand
#define KPE_INCLUDEDOT    0x00000001
#define KPE_INCLUDEDOTDOT 0x00000002
#define KPE_NODIRS        0x00000004
#define KPE_ONLYDIRS      0x00000008

// Structs and fields used for comparison of paths, for the purposes
//   of sorting into a particular order

typedef enum
  {
  KPSORTDIR_ASCENDING = 0,
  KPSORTDIR_DESCENDING = 1
  } KPathSortDir;

typedef enum
  {
  KPSORTFIELD_NAME = 0,
  KPSORTFIELD_MTIME = 1,
  KPSORTFIELD_SIZE = 2,
  } KPathSortField;

typedef enum
  {
  KPSORTGROUPING_DIRSFIRST = 0,
  KPSORTGROUPING_FILESFIRST = 1,
  KPSORTGROUPING_MIXED = 2
  } KPathSortGrouping;

typedef struct _KPathSortStruct 
  {
  KPathSortDir dir;
  KPathSortField field;
  KPathSortGrouping grouping;
  } KPathSortStruct;



typedef enum 
  {
  KPT_UNKNOWN = -1, // Unknown; error
  KPT_REG = 0,      // regular file
  KPT_SOCK = 1,     // Socket
  KPT_LNK = 2,      // Symbolic link
  KPT_BLK = 3,      // Block device
  KPT_CHR = 4,      // Character device
  KPT_FIFO = 5,     // FIFO (pipe)
  KPT_DIR = 6       // Directory
  } KPathType;


struct KPath;
typedef struct _KPath KPath;

BEGIN_DECLS

extern KPath   *kpath_clone (const KPath *s);
extern KPath   *kpath_new_from_utf8 (const UTF8 *path);
// Get the value of $HOME. If $HOME is not set, assume root 
extern KPath   *kpath_new_home (void);
extern void     kpath_destroy (KPath *self);

/** A function that can be passed to klist_sort, to sort a list of KPath
    objects into alphabetic order */
int kpath_sort_fn (const void *p1, const void *p2, 
                     void *user_data);

extern void     kpath_append (KPath *self, const KPath *s);
extern void     kpath_append_utf8 (KPath *self, const UTF8 *s);
extern void     kpath_append_utf32 (KPath *self, const UTF32 *s);

/** Tests with the path ends specifically with a forward-slash, not
    a platform-specific separator. Sometimes separators are forward
    slashes even on platforms where this is not usually the case. */
extern BOOL     kpath_ends_with_fwd_slash (const KPath *self);
extern BOOL     kpath_ends_with_separator (const KPath *self);

// Returns NULL and errno set if expansion fails
extern KList   *kpath_expand (const KPath *self, uint32_t flags);

/** Create the specified directory, and any parent directories that
    are necessary. */
extern BOOL     kpath_create_directory (const KPath *self);

extern FILE    *kpath_fopen (const KPath *self, const char *mode);

extern KPathType kpath_get_type (const KPath *self);

extern KString *kpath_get_filename (const KPath *self);

extern BOOL     kpath_lstat (const KPath *self, struct stat *sb);

extern BOOL     kpath_mtime (const KPath *self, time_t *size);

extern int      kpath_open_read (const KPath *self);
// Open for write, create, truncate, with default mode
extern int      kpath_open_write (const KPath *self);

extern KBuffer *kpath_read_to_buffer (const KPath *self);
extern KString *kpath_read_to_string (const KPath *self);

/** Remove the filename path of a path, if there is one. This method 
  specifically does not require the path to exist -- it works entirely
  on the name pattern. This makes it possible to use in cases where
  we need to create a file and a directory for it to go it, but it
  means that there are certain ambiguous cases.
  Any path that ends in a separator is assumed to be a directory, and
  is not altered. */
extern void     kpath_remove_filename (KPath *self);

extern BOOL     kpath_size (const KPath *self, uint64_t *size);
extern BOOL     kpath_stat (const KPath *self, struct stat *sb);
extern UTF8    *kpath_to_utf8 (const KPath *self);

/** Unlink (delete) the file. This won't work for directories. */
extern BOOL     kpath_unlink (const KPath *self);

END_DECLS

