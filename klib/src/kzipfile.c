/*============================================================================
 
  klib 

  kzipfile.c

  Copyright (c)2017-2020 Kevin Boone, GPL v3.0

============================================================================*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include <klib/defs.h> 
#include <klib/types.h> 
#include <klib/klog.h> 
#include <klib/kzipfile.h> 
#include <klib/klist.h> 
#include <klib/kpath.h> 
#include <klib/kstring.h> 

#define KLOG_CLASS "klib.kzipfile"

struct _KZipFile
  {
  char *filename;
  KList *contents; // List of struct ZipHeader
  }; 


typedef struct _ZipHeader
  {
  int version;
  int flags;
  char filename[PATH_MAX];
  uint64_t compressed_size;
  uint64_t uncompressed_size;
  uint64_t local_header;
  uint64_t data_start;
  uint64_t next_header;
  uint64_t external_attr;
  unsigned int mode;
  int method;
  } ZipHeader;

/*==========================================================================

  kzipfile_new

*==========================================================================*/
KZipFile *kzipfile_new (const KPath *path)
  {
  KLOG_IN
  UTF8 *temp = kstring_to_utf8 ((KString *)path);
  KZipFile *ret = kzipfile_new_utf8 (temp);
  free (temp);
  KLOG_OUT
  return ret;
  }

/*==========================================================================

  kzipfile_new_utf8

*==========================================================================*/
KZipFile *kzipfile_new_utf8 (const UTF8 *filename)
  {
  KLOG_IN
  KZipFile *self = malloc (sizeof (KZipFile));
  self->filename = strdup ((char *)filename);
  self->contents = NULL;
  KLOG_OUT
  return self;
  }

/*==========================================================================

  kzipfile_destroy

  Clean up any memory used by the object. It is safe to call this even
  if the object has not been initialized.

*==========================================================================*/
void kzipfile_destroy (KZipFile *self)
  {
  KLOG_IN
  if (self)
    {
    if (self->filename) free (self->filename);
    if (self->contents) klist_destroy (self->contents);
    free (self);
    }
  KLOG_OUT
  }

/*==========================================================================

  kzipfile_read_local_header

  Read a zip local header, assuming that the file pointer is already in
    the right place. The only thing we really need to get from the local
    header is the start of the compressed data. Unfortunately, this is
    not stored, but calculated from the size of the local header, which is
    (sigh) variable. So the whole process is far more complicated than it
    needs to be.

*==========================================================================*/
ZipError kzipfile_read_local_header (int f, ZipHeader *h)
  {
  KLOG_IN
  int error = ZE_OK;
  unsigned char buff[PATH_MAX];
  uint64_t offset = lseek (f, 0, SEEK_CUR);
  if (read (f, buff, 30) == 30)
    {
    if (buff[0] == 0x50 && buff[1] == 0x4B && buff[2] == 0x03
         && buff[3] == 0x04)
      {
      int version = buff[4] + 256 * buff[5];
      h->version = version;
      klog_trace (KLOG_CLASS, "version = %d", version);
      int flags = buff[6] + 256 * buff[7];
      int method = buff[8] + 256 * buff[9];
      h->flags = flags;
      h->method = method;
      klog_trace (KLOG_CLASS, "flag = %08x", flags);
      uint64_t compressed_size = buff[18] + 256 * buff[19] + 
         256 * 256 * buff[20] + 256 * 256 * 256 * buff[21];
      h->compressed_size = compressed_size;
      uint64_t uncompressed_size = buff[22] + 256 * buff[23] + 
         256 * 256 * buff[24] + 256 * 256 * 256 * buff[25];
      h->uncompressed_size = uncompressed_size;
      int filename_len = buff[26] + 256 * buff[27];
      int extra_len = buff[28] + 256 * buff[29];
      read (f, buff, filename_len);
      buff[filename_len] = 0;
      strcpy (h->filename, (char *)buff);
      klog_trace (KLOG_CLASS, "filename=%s", h->filename);
      klog_trace (KLOG_CLASS, "extra = %d", extra_len);
      klog_trace (KLOG_CLASS, "comp = %lld", (long long int)h->compressed_size);
      klog_trace (KLOG_CLASS, "uncomp = %lld", (long long int)h->uncompressed_size);
      klog_trace (KLOG_CLASS, "method = %d", h->method);
      h->data_start = offset + 30 + filename_len + extra_len;
      h->next_header = h->data_start + h->compressed_size; 
      klog_trace (KLOG_CLASS, "data start = %ld", h->data_start);
      BOOL has_dd = FALSE;
      if (flags & 0x08)
	has_dd = TRUE;
      if (has_dd)
        {
	// Ugh! This really sucks. If the entry has a 'data descriptor',
	//   then the compressed size and checksum are written in a 
	//   separate block _after_ the compressed data
	// To make it worse, this block is not of a fixed size
        klog_trace (KLOG_CLASS, "Entry has DD");
	uint64_t old_pos = lseek (f, 0, SEEK_SET);
	lseek (f, h->data_start + h->compressed_size, SEEK_SET);
        read (f, buff, 16);
	int off = 8;
        if (buff[0] == 0x50 && buff[1] == 0x4B && buff[2] == 0x07
             && buff[3] == 0x08)
           {
           klog_trace (KLOG_CLASS, "DD has signature");
	   off = 12;
           }

        uint64_t uncompressed_size = buff[off] + 256 * buff[off+1] + 
           256 * 256 * buff[off+2] + 256 * 256 * 256 * buff[off+3];
        h->uncompressed_size = uncompressed_size;

	lseek (f, old_pos, SEEK_SET);
	h->next_header += off + 4;
	}
      klog_trace (KLOG_CLASS, "next header = %ld", h->next_header);
      }
    else if (buff[0] == 0x50 && buff[1] == 0x4B && buff[2] == 0x01
         && buff[3] == 0x02)
      {
      // We've reached the central directory. This is not an error, 
      //   except if it is the first header in the file -- and
      //   that isn't strictly speaking an error, it's just an empty
      //   file
      error = ZE_CD;
      }
    else
      {
      klog_warn (KLOG_CLASS, "zip_read_header: bad magic number");
      error = ZE_BADZIP;
      }
    }
  else
    {
    klog_warn (KLOG_CLASS, "zip_read_header: file too short");
    error = ZE_BADZIP;
    }

  KLOG_OUT
  return error;
  }


/*==========================================================================

  kzipfile_read_header_from_cd

  Read a file header from the central directory, assuming the file pointer
    is in the right place. We can get all the information we need about
    a compressed file from this place, _except_ where the data is actually
    stored on disk. The central directory stores a pointer to the 'local
    header' (which is of variable size); the compressed data starts after
    that.

*==========================================================================*/
ZipError zip_read_header_from_cd (int f, ZipHeader *h)
  {
  KLOG_IN
  int error = ZE_OK;
  unsigned char buff[46];
  uint64_t offset = lseek (f, 0, SEEK_CUR);
  if (read (f, buff, 46) > 20)
    {
    if (buff[0] == 0x50 && buff[1] == 0x4B && buff[2] == 0x01
         && buff[3] == 0x02)
      {
      uint64_t compressed_size = buff[20] + 256 * buff[21] + 
         256 * 256 * buff[22] + 256 * 256 * 256 * buff[23];
      uint64_t uncompressed_size = buff[24] + 256 * buff[25] + 
         256 * 256 * buff[26] + 256 * 256 * 256 * buff[27];
      uint64_t filename_length = buff[28] + 256 * buff[29];
      uint64_t extra_length = buff[30] + 256 * buff[31];
      uint64_t comment_length = buff[32] + 256 * buff[33];
      uint64_t external_attr = buff[38] + 256 * buff[39] + 
         256 * 256 * buff[40] + 256 * 256 * 256 * buff[41];
      uint64_t local_header = buff[42] + 256 * buff[43] + 
         256 * 256 * buff[44] + 256 * 256 * 256 * buff[45];
      h->compressed_size = compressed_size;
      h->uncompressed_size = uncompressed_size;
      h->local_header = local_header;
      h->external_attr = external_attr;
      h->mode = (external_attr >> 16) & 0777; 
      klog_debug (KLOG_CLASS, "Compressed size = %ld", h->compressed_size);
      klog_debug (KLOG_CLASS, "Uncompressed size = %ld", h->uncompressed_size);

      char buff[PATH_MAX];
      read (f, buff, filename_length);
      buff[filename_length] = 0;
      strcpy (h->filename, (char *)buff);

      h->next_header = offset + 46 + filename_length + extra_length + 
          comment_length;
   
      lseek (f, local_header, SEEK_SET);
      ZipHeader lh;
      kzipfile_read_local_header (f, &lh);
      h->method = lh.method;
      h->data_start = lh.data_start;

      // We can seek to the local header now, because next_header is
      //  already stored for this CD entry
      }
    else if (buff[0] == 0x50 && buff[1] == 0x4B && buff[2] == 0x05
         && buff[3] == 0x06)
      {
      // Reached the end of the CD
      error = ZE_CD;
      }
    else
      {
      error = ZE_BADZIP;
      klog_debug (KLOG_CLASS, "Unrecognized CD signature");
      }
    }
  else
    {
    // It's an error if we hit EOF without encountering the end-of-CD
    //   signature (although we could excuse this, I guess, in 
    //   necessary)
    error = ZE_BADZIP;
    }

  KLOG_OUT
  return error;
  }

/*==========================================================================

  kzipfile_read_cd

  Read the central directory, creating a List of struct ZipHeader as
   we go. The List may legitimately be empty at the end -- it is not
   actually an error for a kzipfile to contain no files (but it must
   contain a CD). 

*==========================================================================*/
ZipError kzipfile_read_cd (KZipFile *self, uint64_t cd)
  {
  KLOG_IN
  ZipError error = 0;

  klog_debug (KLOG_CLASS, "kzipfile_read_cd: %s, %ld", self->filename, cd);
  int f = open (self->filename, O_RDONLY);
  if (f > 0)
    {
    lseek (f, cd, SEEK_SET);
    struct stat sb;
    fstat (f, &sb);
    uint64_t filesize = sb.st_size;
    ZipHeader h;
    error = zip_read_header_from_cd (f, &h); 
    if (!error)
      {
      self->contents = klist_new_empty (free);
      ZipHeader *hh = malloc (sizeof (ZipHeader));
      memcpy (hh, &h, sizeof (ZipHeader));
      klist_append (self->contents, hh);
      do
	{
	if (h.next_header < filesize)
	  {
	  lseek (f, h.next_header, SEEK_SET);
          error = zip_read_header_from_cd (f, &h); 
	  if (!error)
	    {
            ZipHeader *hh = malloc (sizeof (ZipHeader));
            memcpy (hh, &h, sizeof (ZipHeader));
            klist_append (self->contents, hh);
	    }
	  }
        } while (!error);
      if (error == ZE_CD) error = 0;
      }
    close (f);
    }
  else
    {
    klog_debug (KLOG_CLASS, "kzipfile_find_cd: can't open %s for reading", 
      self->filename);
    return ZE_OPENREAD;
    }

  KLOG_OUT
  return error;
  }


/*==========================================================================

  kzipfile_find_cd

  Find the central directory at the end of the kzipfile. This is very ugly,
    but the zip file format does not provide any elegant way to find the
    CD. We have to read the last 64k, and hunt for the signature of the
    end-central-directory record. 64k is the largest this can be but, in
    fact, it will usually be in the last hundred bytes or so. 
  The end-central-directory contains the offset of the first CD header.

*==========================================================================*/
ZipError kzipfile_find_cd (KZipFile *self, uint64_t *cd)
  {
  KLOG_IN
  ZipError ret = 0;

  klog_debug (KLOG_CLASS, "kzipfile_read_cd: %s", self->filename);
  int f = open (self->filename, O_RDONLY);
  if (f > 0)
    {
    struct stat sb;
    fstat (f, &sb);
    uint64_t filesize = sb.st_size;
    int toread, tostart;
    if (filesize > 65536)
      {
      toread = 65536;
      tostart = filesize - 65536;
      }
    else
      {
      toread = filesize;
      tostart = 0;
      }
    unsigned char *buff = malloc (toread);
    lseek (f, tostart, SEEK_SET);     
    read (f, buff, toread);
    //int n = read (f, buff, toread);

    for (int i = 0; i < toread - 4 && (*cd == -1); i++)
      {
      BYTE b = buff[i];
      if (b == 0x50)
        {
        if (buff[i+1] == 0x4b && buff[i+2] == 0x05 && buff[i+3] == 0x06)
          {
          *cd = (int)buff[i + 16] + 256 * buff[i + 17] + 
             256 * 256 * buff[i + 18] + 256 * 256 * 256 * buff[i + 19];
          klog_debug (KLOG_CLASS, "Found CD at %ld", *cd);
          }
        }
      }
    
    free (buff);
    close (f);
    }
  else
    {
    klog_debug (KLOG_CLASS, "kzipfile_find_cd: can't open %s for reading", 
      self->filename);
    ret = ZE_OPENREAD;
    }

  KLOG_OUT
  return ret;
  }

/*==========================================================================

  kzipfile_read_contents

  Read the kzipfile metadata and build an index. This must be the 
   first method called after the KZipFile object is created.

*==========================================================================*/
ZipError kzipfile_read_contents (KZipFile *self)
  {
  int error = ZE_OK;
  KLOG_IN

  uint64_t cd = -1;
  error = kzipfile_find_cd (self, &cd);
  if (!error)
    error = kzipfile_read_cd (self, cd);

  KLOG_OUT
  return error;
  }

/*==========================================================================

  kzipfile_get_num_entries

  Get the number of entries in the index, including zero-length entries
    (which are often directories).

*==========================================================================*/
int kzipfile_get_num_entries (const KZipFile *self)
  {
  return klist_length (self->contents);
  }


/*==========================================================================

  kzipfile_get_entry_details

  Get the size and filename of an entry.

  Note that filename may be a path. It may also be a directory, 
    conventionally indicated by a trailing '/' and zero size.

*==========================================================================*/
void kzipfile_get_entry_details (const KZipFile *self, int n, char *filename, 
       int max_filename, uint64_t *size)
  {
  KLOG_IN

  int l = kzipfile_get_num_entries (self);
  if (n >= l)
    klog_error 
       (KLOG_CLASS, 
         "kzipfile_get_entry_details: attempt to reference non-existent entry:"
           " %d of %d", n, l);
  else
    {
    ZipHeader *h = klist_get (self->contents, n);
    strncpy (filename, h->filename, max_filename);
    *size = h->uncompressed_size;
    }

  KLOG_OUT
  }


/*==========================================================================

  kzipfile_extract_to_memory

  Extract the entry to a block of memory, which will be allocated.
  If the method returns error, no data will have been allocated. Otherwise,
   the caller must free it.
 
  If this method is applied to an entry of zero size, which might be a
   directory, it will allocate an empty buffer -- this is not an error,
   although it may well be unhelpful.

*==========================================================================*/
ZipError kzipfile_extract_to_memory (const KZipFile *self, int n, 
    BYTE **out, uint64_t *length)
  {
  KLOG_IN

  ZipError ret = ZE_OK;

  klog_debug (KLOG_CLASS, "zip_extract_to_memory, entry %d", n);

  int l = kzipfile_get_num_entries (self);
  if (n >= l)
    {
    klog_error 
       (KLOG_CLASS, 
         "zip_extract_to_file: attempt to reference non-existent entry:"
           " %d of %d", n, l);
     ret = ZE_INTERNAL;
     }
  else
    {
    ZipHeader *h = klist_get (self->contents, n);
    int method = h->method;
    if (method == 8 || method == 0)
      {
      int f = open (self->filename, O_RDONLY);
      if (f >= 1)
        {
        uint64_t data_start = h->data_start;
	lseek (f, data_start, SEEK_SET);
	if (method == 0) // uncompressed
	  {
          klog_debug (KLOG_CLASS, "kzipfile_extract_to_memory: %s: entry is uncompressed",
             self->filename);
	  *out = malloc (h->uncompressed_size);
	  read (f, *out, h->uncompressed_size);
	  }
	else // DEFLATE
	  {
          klog_debug (KLOG_CLASS, "kzipfile_extract_to_memory: %s: entry is deflated");
	  // No point checking these mallocs -- they will always 
	  //   succeed on Linux, even in low memory 
	  BYTE *in = malloc (h->compressed_size + 2);
	  *out = malloc (h->uncompressed_size);
	  read (f, in + 2, h->compressed_size);
          in[0] = 0x78; // Zlib magic numbers
          in[1] = 0x9C;

	  uint64_t uncompressed_size = h->uncompressed_size;
	  uint64_t compressed_size = h->compressed_size + 2;
	  uncompress2 (*out, &uncompressed_size,
	      in, &compressed_size);
          free (in);
          if (uncompressed_size != h->uncompressed_size)
            {
            free (*out);
            ret = ZE_CORRUPT;
            }
	  }
        close (f);
        if (length) *length = h->uncompressed_size;
	}
      else
        {
        ret = ZE_OPENREAD;
        klog_warn (KLOG_CLASS, "kzipfile %s seems to have vanished", self->filename);
	}
      }
    else
      {
      klog_warn (KLOG_CLASS, "Unsupported compression method %d in %s",
        method, self->filename);
      ret = ZE_UNSUPPORTED_COMP;
      }
    }

  KLOG_OUT

  return ret;

  }


/*==========================================================================

  kzipfile_extract_to_file

  This method will overwrite the file without warning!

  Attempting to extract an entry which is a directory will result in 
    zero-size file being created with the same name as the directory.
  See kzipfile_extract_all() to see how to avoid this behaviour if 
    necessary. This method tries to restore file permissions, if they
    were stored in the zip in a recognized way. It is not considered
    and error if file permissions cannot be restored.

*==========================================================================*/
ZipError kzipfile_extract_to_file (const KZipFile *self, int n, 
    const char *filename)
  {
  KLOG_IN

  ZipError ret = ZE_OK;

  klog_debug (KLOG_CLASS, "zip_extract_to_file, entry %d, file %s", n, filename);

  int l = kzipfile_get_num_entries (self);
  if (n >= l)
    {
    klog_error 
       (KLOG_CLASS, 
        "zip_extract_to_file: attempt to reference non-existent entry:"
           " %d of %d", n, l);
     ret = ZE_INTERNAL;
     }
  else
    {
    ZipHeader *h = klist_get (self->contents, n);
    int method = h->method;
    if (method == 8 || method == 0)
      {
      int fo = open (filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
      if (fo >= 1)
        {
	BYTE *out = NULL;
	ret = kzipfile_extract_to_memory (self, n, &out, NULL);
	if (out) 
	  {
	  write (fo, out, h->uncompressed_size);
          free (out);
	  }
        close (fo);
	}
      else
	{
	ret = ZE_OPENWRITE;
	}
      }
    else
      {
      klog_warn (KLOG_CLASS, "Unsupported compression method %d in %s",
        method, self->filename);
      ret = ZE_UNSUPPORTED_COMP;
      }
    }

  KLOG_OUT

  return ret;
  }


/*==========================================================================

  kzipfile_extract_all

  Extract the contents of the kzipfile, with a path based on extract_path.
  This method will create directories that are specified in the kzipfile,
    and also such directories as are necessary to hold the files 
    extracted. This method always respects the stored file structure, if
    there is one. 
  If carry_on is set, then the method will try to continue even if there
    is an error. In such a case, the return value cannot be trusted.

*==========================================================================*/
int kzipfile_extract_all (const KZipFile *self, const char *extract_path,
      BOOL carry_on)
  {
  KLOG_IN
  int ret = ZE_OK;

  klog_debug (KLOG_CLASS, "kzipfile_extract_all, to %s\n", extract_path);
  KList *contents = self->contents;
  int l = klist_length (contents);
  for (int i = 0; (ret == 0 || carry_on) && i < l; i++)
    {
    ZipHeader *ze = klist_get (contents, i);
    KPath *path = kpath_new_from_utf8 ((UTF8*)extract_path);
    kpath_append_utf8 (path, (UTF8*)ze->filename);
    // Zip format uses an entry ending in / to indicate a directory
    if (kpath_ends_with_fwd_slash (path))
      {
      // If the kzipfile entry is for a directory, we should create it,
      //   even if it turns out to be empty. However, we can't rely
      //   on the zipper creating directory entries -- the program
      //   has to willing to infer them from pathnames
      char *s_path = (char *)kpath_to_utf8 (path);
      free (s_path);
      if (kpath_create_directory (path))
        {
        }
      else
        {
        klog_debug (KLOG_CLASS, "zip_extract_all: could not create directory %s\n", 
          s_path); 
        }
      }
    else
      {
      KPath *newpath = kpath_clone (path);
      kpath_remove_filename (newpath);  
      kpath_create_directory (newpath);
      kpath_destroy (newpath);

      char *s_path = (char *)kpath_to_utf8 (path);
      ret = kzipfile_extract_to_file (self, i, s_path); 
      if (ze->mode) 
       {
       chmod (s_path, ze->mode);
       }
      free (s_path);
      }
    kpath_destroy (path);
    } 

  KLOG_OUT
  return ret;
  }


/*==========================================================================

  kzipfile_extract_to_buffer

  Extract entry n to a Buffer, which will be allocated. It is not 
  considered an error to extract a directory this way -- it is just 
  another zero-length file. Callers should ensure that the entry is
  not a directory (e.g., by checking whether the filename ends in /)
  if that behavioue is undesirable. Callers should free the allocated
  buffer after use. If the return value is an error, no buffer will have
  been allocated.

*==========================================================================*/
ZipError kzipfile_extract_to_buffer (const KZipFile *self, int n, 
     KBuffer **buffer)
  {
  KLOG_IN
  BYTE *out = NULL;
  uint64_t length = 0;
  ZipError err = kzipfile_extract_to_memory (self, n, 
    &out, &length);
  if (!err) 
    {
    KBuffer *buff = kbuffer_new_from_data_no_copy (out, length);
    // Don't free 'out' -- it belongs to the buffer now
    *buffer = buff;
    }

  KLOG_OUT
  return err;
  }

/*==========================================================================
  kzipfile_get_filename
*==========================================================================*/
const char *kzipfile_get_filename (const KZipFile *self)
  {
  return self->filename;
  }

