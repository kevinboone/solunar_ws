/*==========================================================================

  boilerplate
  numberformat.c
  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

==========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <klib/kstring.h> 
#include <klib/klog.h> 
#include <klib/numberformat.h> 

#define KLOG_CLASS "klib.numberformat"

/*==========================================================================

  numberformat_eng

==========================================================================*/
UTF8* numberformat_eng (double v)
  {
  KLOG_IN

  BOOL free_suffix = FALSE;
  double m;
  int e;
  numberformat_eng_reduce (v, &m, &e);
  char *suffix = NULL;
  switch (e)
    {
    case 0: suffix = ""; break;
    case 3: suffix = "k"; break;
    case 6: suffix = "M"; break;
    case 9: suffix = "G"; break;
    case 12: suffix = "T"; break;
    case 15: suffix = "P"; break;
    case -3: suffix = "m"; break;
    case -6: suffix = "μ"; break;
    case -9: suffix = "n"; break;
    case -12: suffix = "p"; break;
    case -15: suffix = "f"; break;
    default: asprintf (&suffix, "E%d", e); free_suffix = TRUE;
    }
 
  char *ret;
  asprintf (&ret, "%1.3G%s", m, suffix);
  if (free_suffix) free (suffix);

  KLOG_OUT
  return (UTF8 *)ret;
  }


/*==========================================================================

  numberformat_eng_reduce 

==========================================================================*/
void numberformat_eng_reduce (double v, double *mantissa, int *exponent)
  {
  KLOG_IN

  if (v == 0) 
    {
    *mantissa = 0; *exponent = 0;
    }
  else
    {
    BOOL neg = (v < 0);
    if (neg) v = -v;
    double exp = log10 (v);
    int iexp = (int)exp;
    if (exp < 0)
      iexp -= 3;
    iexp = iexp / 3 * 3;
    *exponent = iexp;
     
    *mantissa = v / (pow (10, iexp));
    if (neg) *mantissa = -*mantissa;
    }
  
  
  KLOG_OUT
  }


/*==========================================================================

  numberformat_size_64

==========================================================================*/
char *numberformat_size_64 (uint64_t n, const char *sep, BOOL binary)
  {
  KLOG_IN
  char *ret;

  uint64_t kil = binary ? 1024 : 1000; 

  char *ksuff = binary ? "Ki" : "K";
  char *msuff = binary ? "Mi" : "M";
  char *gsuff = binary ? "Gi" : "G";
  char *tsuff = binary ? "Ti" : "T";

  uint64_t meg = kil * kil; 
  uint64_t gig = kil * meg; 
  uint64_t ter = kil * gig; 

  if (1) // For future expansion
    {
    if (n < 1024) 
      {
      char *out= numberformat_space_64 (n, sep);
      asprintf (&ret, "%s", out);
      }
    else if (n < meg)
      {
      uint64_t iwhole = n / kil;
      uint64_t ifrac = 100 * (n % kil) / kil; 
      uint64_t t = 1000 * (n % kil) / kil - 10 * ifrac; 
      if (t >= 5) ifrac++;
      char *whole = numberformat_space_64 (iwhole, sep);
      asprintf (&ret, "%s.%02d %s", whole, (int)ifrac, ksuff);
      free (whole); 
      }
    else if (n < gig)
      {
      uint64_t iwhole = n / meg;
      uint64_t ifrac = 100 * (n % meg) / meg; 
      uint64_t t = 1000 * (n % meg) / meg - 10 * ifrac; 
      if (t >= 5) ifrac++;
      char *whole = numberformat_space_64 (iwhole, sep);
      asprintf (&ret, "%s.%02d %s", whole, (int)ifrac, msuff);
      free (whole); 
      }
    else if (n < ter)
      {
      uint64_t iwhole = n / gig;
      uint64_t ifrac = 100 * (n % gig) / gig; 
      uint64_t t = 1000 * (n % gig) / gig - 10 * ifrac; 
      if (t >= 5) ifrac++;
      char *whole = numberformat_space_64 (iwhole, sep);
      asprintf (&ret, "%s.%02d %s", whole, (int)ifrac, gsuff);
      free (whole); 
      }
    else 
      {
      uint64_t iwhole = n / ter;
      uint64_t ifrac = 100 * (n % ter) / ter; 
      uint64_t t = 1000 * (n % ter) / ter - 10 * ifrac; 
      if (t >= 5) ifrac++;
      char *whole = numberformat_space_64 (iwhole, sep);
      asprintf (&ret, "%s.%02d %s", whole, (int)ifrac, tsuff);
      free (whole); 
      }
    }

  KLOG_OUT
  return ret;
  }


/*==========================================================================

  numberformat_space_64

==========================================================================*/
char *numberformat_space_64 (int64_t n, const char *sep)
  {
  KLOG_IN

  char *ret;
  if (n == 0)
    {
    ret = strdup ("0");
    }
  else
    {
    BOOL neg = FALSE;
    if (n < 0) 
      {
      neg = TRUE;
      n = -n;
      }

    char in[30];
    ret = malloc (40);
    ret[0] = 0;
    sprintf (in, "%ld", n);
    int lin = strlen (in);

    int before = (lin % 3);

    for (int i = 0; i < lin; i++)
      {
      char t[2]; t[0] = in[i]; t[1] = 0;
      strncat (ret, t, 2);
      if (sep)
        {
        if ((before - 1) % 3 == 0 && i != lin - 1)
          {
          strcat (ret, sep);
          }
        before--;
        }
      }

    if (neg)
      {
      char *out2 = malloc (strlen (ret) + 2);
      strcpy (out2 + 1, ret);
      out2[0] = '-';
      free (ret);
      ret = out2;
      }
    }

  KLOG_OUT 
  return ret; 
  }


/*==========================================================================

  numberformat_read_integer

==========================================================================*/
BOOL numberformat_read_integer (const char *s, uint64_t *v, BOOL strict)
  {
  KLOG_IN
  BOOL ret = FALSE;
  if (strlen (s) == 0)
    { 
    // Empty -- no way it's a number
    ret = FALSE;
    }
  else
    {
    char first = s[0];
    if ((first >= '0' && first <= '9') || first == '+' || first == '-' 
         || !strict)
      {
      char *endp;
      uint64_t n = strtoll (s, &endp, 10);
      if (endp == s) 
        {
        ret = FALSE; // No digits
        }
      else
        {
        if (*endp == 0)
          {
          ret = TRUE;
          }
        else
          {
          // We read _some_ digits, so this is valid except
          //   in strict mode
          ret = !strict;
          }
        }
      *v = n;
      }
    }
  KLOG_OUT
  return ret;
  }


/*==========================================================================

  numberformat_read_double

==========================================================================*/
BOOL numberformat_read_double (const char *s, double *v, BOOL strict)
  {
  KLOG_IN
  BOOL ret = FALSE;
  if (strlen (s) == 0)
    { 
    // Empty -- no way it's a number
    ret = FALSE;
    }
  else
    {
    char first = s[0];
    if ((first >= '0' && first <= '9') || first == '+' || first == '-' 
        || first == '.' || first == ','  || !strict)
      {
      char *endp;
      double n = strtod (s, &endp);
      if (endp == s) 
        {
        ret = FALSE; // No digits
        }
      else
        {
        if (*endp == 0)
          {
          ret = TRUE;
          }
        else
          {
          // We read _some_ digits, so this is valid except
          //   in strict mode
          ret = !strict;
          }
        }
      *v = n;
      }
    }
  KLOG_OUT
  return ret;
  }



