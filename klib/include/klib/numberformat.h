/*============================================================================
  
  klib
  
  numberformat.h 
  
  Functions for reading and writing numbers in various different
  formats. Some functions in the file require "-lm"
  
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <stdint.h>

BEGIN_DECLS

/**  Format a 64-bit (or smaller) integer by separating the digits into
     groups of three. The separator can be any sequence of unicode or
     ASCII bytes, It's safe to supply a NULL for the separator, which 
     is useful in cases where this function is called by other 
     number-formatting routines that may, or may not, require separators. */
char   *numberformat_space_64 (int64_t n, const char *sep);

/** Format a 64-bit unsigned number, assumed to be the size of something
     (file, disk), so that if possible it is expressed using only three digits 
     (e.g., 1.23KB) and a suffix. 
   If 'binary' is TRUE, the units are KiB, MiB, GiB, and TiB, which are all 
     integer powers of two. Otherwise, the units are KB, MB, GB, and TB, 
     all integer powers of 10. Note that in decimal mode, 1KB=1000B, not 1024, 
     as is the modern practice. Nearly all software that displays sizes now 
     uses the IEC standard binary units -- GiB, TiB, etc -- with the 
     exception of Microsoft.
   If sep is non-null, then numbers over 1000 are separated into groups of 
     three digits using the supplied separator. This will only happen in 
     binary mode.  */
char   *numberformat_size_64 (uint64_t n, const char *sep, BOOL binary);

/**  Read a 64-bit decimal integer from a string. If strict is TRUE, then
      the entire string must consist of digits, optionally preceded by
      +  or -. If FALSE, there may be any amount of whitespace at the start 
      of the number, and any amount of anything at the end.
     This function can't read hexadecimal numbers, but (weirdly) 
       numberformat_read_double can. */
BOOL    numberformat_read_integer (const char *s, uint64_t *v, BOOL strict);

/**  Read a decimal floating point number from a string. If strict is TRUE, 
       then the number must start at the beginning of the string, and extend 
       to the end. If FALSE, there may be any amount of whitespace at the start 
       of the number, and any amount of anything at the end.
     This function understands scientific notation ("1.2E3"). The decimal
       separator is locale-specific.
     By a weird quirk of the way strtod is implemented, this function
       will also read a hexadecimal number, if it starts with 0x.*/
BOOL    numberformat_read_double (const char *s, double *v, BOOL strict);

/** Convert a number to a mantissa and exponent, where the exponent is
    an integer power of three. Note that exponents are not symmetrical
    about exponent zero; that is, 0.1 is 100E-3, while 10 is 1E1 This
    is standard engineering practice -- a number less than one, but
    greater than 10-3, is a milli-something. */
void    numberformat_eng_reduce (double v, double *mantissa, int *exponent);

/** Format a number in engineering notation -- if the exponent is < 15,
    use one of the standard engineering suffices -- m, k, μ, etc.  If the
    exponent is not in this range, write "E23" or "E-50" or whatever. Note 
    that this function is defined to return UTF8*, not char*, as a reminder 
    that the result may contain multi-byte characters.  As is common practice, 
    the mantissa is displayed to no more than three significant figures. */
UTF8   *numberformat_eng (double v);

END_DECLS



