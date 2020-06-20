/*============================================================================
  
  klib
  
  klog.c

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
#include <klib/klog.h>

// Fwd refs
static void klog_v (KLogLevel level, const char *cls, const char *fmt, 
         va_list ap);

static int log_level = KLOG_INFO;

static KLogHandler log_handler = NULL;

static void *log_user_data = NULL;

/*============================================================================
  
  klog_debug

  ==========================================================================*/
void klog_debug (const char *cls, const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  klog_v (KLOG_DEBUG, cls, fmt, ap);
  va_end (ap);
  }

/*============================================================================
  
  klog_error

  ==========================================================================*/
void klog_error (const char *cls, const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  klog_v (KLOG_ERROR, cls, fmt, ap);
  va_end (ap);
  }

/*============================================================================
  
  klog_info 

  ==========================================================================*/
void klog_info (const char *cls, const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  klog_v (KLOG_INFO, cls, fmt, ap);
  va_end (ap);
  }

/*============================================================================
  
  klog_level_to_utf8

  ==========================================================================*/
const UTF8 *klog_level_to_utf8 (KLogLevel level)
  {
  char *ret = "";
  switch (level)
    {
    case KLOG_ERROR: ret = "ERROR"; break;
    case KLOG_WARN: ret = "WARN"; break;
    case KLOG_INFO: ret = "INFO"; break;
    case KLOG_DEBUG: ret = "DEBUG"; break;
    case KLOG_TRACE: ret = "TRACE"; break;
    }
  return (UTF8*) ret; 
  }

/*============================================================================
  
  klog_init

  ==========================================================================*/
void klog_init (KLogLevel level, KLogHandler handler, void *user_data)
  {
  log_level = level;
  log_handler = handler;
  log_user_data = user_data;
  }

/*============================================================================
  
  klog_warn

  ==========================================================================*/
void klog_warn (const char *cls, const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  klog_v (KLOG_WARN, cls, fmt, ap);
  va_end (ap);
  }

/*============================================================================
  
  klog_set_handler

  ==========================================================================*/
extern void klog_set_handler (KLogHandler handler)
  {
  log_handler = handler;
  }

/*============================================================================
  
  klog_set_log_level

  ==========================================================================*/
void klog_set_log_level (int level)
  {
  log_level = level;
  }

/*============================================================================
  
  klog_trace

  ==========================================================================*/
void klog_trace (const char *cls, const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  klog_v (KLOG_TRACE, cls, fmt, ap);
  va_end (ap);
  }

/*===========================================================================
klog_v
============================================================================*/
void klog_v (KLogLevel level, const char *cls, const char *fmt,  
                     va_list ap)
  {
  if (level > log_level) return;
  char *s;
  vasprintf (&s, fmt, ap);
  if (log_handler)
    log_handler (level, cls, log_user_data, s);
  else
    fprintf (stderr, "%s %s: %s\n", klog_level_to_utf8 (level), cls, s);
  free (s);
  }

