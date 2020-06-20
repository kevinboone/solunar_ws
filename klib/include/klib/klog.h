/*============================================================================
  
  klib
  
  klog.h

  Definition of the KString class

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/types.h>
#include <klib/defs.h>

typedef enum 
  {
  KLOG_ERROR = 0,
  KLOG_WARN = 1,
  KLOG_INFO = 2,
  KLOG_DEBUG = 3,
  KLOG_TRACE = 4
  } KLogLevel;

#define KLOG_IN klog_trace(KLOG_CLASS, "Entering %s ", __PRETTY_FUNCTION__);
#define KLOG_OUT klog_trace(KLOG_CLASS, "Leaving %s", __PRETTY_FUNCTION__);

BEGIN_DECLS

typedef void (*KLogHandler) (KLogLevel level, const char *cls, 
                  void *user_data, const char *msg); 

extern void        klog_debug (const char *cls, const char *fmt,...);
extern void        klog_error (const char *cls, const char *fmt,...);
extern void        klog_info (const char *cls, const char *fmt,...);
extern void        klog_init (KLogLevel level, KLogHandler handler, 
                     void *user_data);
extern const UTF8 *klog_level_to_utf8 (KLogLevel level);
extern void        klog_set_handler (KLogHandler handler);
extern void        klog_set_log_level (int level);
extern void        klog_trace (const char *cls, const char *fmt,...);
extern void        klog_warn (const char *cls, const char *fmt,...);

END_DECLS

