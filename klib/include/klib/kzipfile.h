/*==========================================================================

  klib
 
  kzipfile.h

  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

  Methods to interrogate and unpack zipfiles. Zips can be extracted to
    new files, or to memory buffers.

  The general use is to call kzipfile_new (filename) to initialse the 
    KZipFile object, and kzipfile_destory to free it. new() does not
    read the file; the next method called will nearly always be
    kzipfile_read_contents(), which initializes the index of metadata. 
  kzipfile_read_contents() can fail, as can any of the subsequent extraction
    methods. In failure, they return one of the ZE_XX error codes. If the
    error is ZE_OPENREAD or ZE_OPENWRITE then errno will also be set; 
    in other cases, errno will not provide any information. All methods in
    this class return ZE_OK, that is zero, on success. 
  The methods that extract to file try to respect the file permissions
    stored in the zipfile, but often these are missing. In that case, the
    permissions are set by the user's unask (so usually end up 644).
  The actual data decompression is carried out by zlib, which must be
    linked with the application.

  Limitations:

  - Encryption is not supported
  - The only compression method supported is 'deflate'
  - Multi-file zips are not supported
  - Checksums are ignored. When extracting a deflated file, the operation
    is considered successful if the extracted data ends up the same size
    as the stored value of the uncompressed size
  - An instance of KZipFile can only be used to process a single zipfile -- 
    it cannot be re-used

==========================================================================*/

#pragma once

#include <stdint.h>
#include <klib/defs.h> 
#include <klib/types.h> 
#include <klib/kbuffer.h> 
#include <klib/kpath.h> 

typedef enum
  {
  ZE_OK = 0,
  ZE_OPENREAD = 1,
  ZE_BADZIP = 2,
  // ZE_CD is an internal code, indicating that we hit the 'end central 
  //  directory' when building the index. It's only an error, and then
  //  only just, if it's the first entry in the file. No method should
  //  return this code to callers
  ZE_CD = 3, 

  // We only support DEFLATE (and uncompressed) entries
  ZE_UNSUPPORTED_COMP = 4,
  ZE_OPENWRITE = 5,
  // Zip structure OK, but compressed data defective in some way
  ZE_CORRUPT = 6,
  ZE_INTERNAL = -1
  } ZipError;

struct _KZipFile;
typedef struct _KZipFile KZipFile;

BEGIN_DECLS

KZipFile *kzipfile_new (const KPath *path);
KZipFile *kzipfile_new_utf8 (const UTF8 *filename);
void      kzipfile_destroy (KZipFile *self);
ZipError  kzipfile_read_contents (KZipFile *self);
int       kzipfile_get_num_entries (const KZipFile *self);
void      kzipfile_get_entry_details (const KZipFile *self, 
           int n, char *filename, int max_filename, uint64_t *size);
ZipError  kzipfile_extract_to_file (const KZipFile *self, int entry, 
           const char *filename);
ZipError  kzipfile_extract_to_memory (const KZipFile *self, int n, 
           BYTE **out, uint64_t *length);
int       kzipfile_extract_all (const KZipFile *self, 
           const char *extract_path, BOOL carry_on);
ZipError  kzipfile_extract_to_buffer (const KZipFile *self, int n, 
            KBuffer **buffer);
const char *kzipfile_get_filename (const KZipFile *self);

END_DECLS


