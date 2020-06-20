/*============================================================================
  
  solunar_ws 
  
  programcontext.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#include <assert.h> 
#include <stdio.h> 
#include <time.h> 
#include <stdlib.h> 
#include <klib/klib.h> 
#include <string.h> 
#include <getopt.h> 
#include "program_context.h" 

#define KLOG_CLASS "solunar_ws.program_context"
void program_context_show_usage (FILE *f, const char *argv0);

/*============================================================================
  
  ProgramContext  

  ==========================================================================*/
struct _ProgramContext
  {
  KProps *props;
  int nonswitch_argc;
  char **nonswitch_argv;
  };

/*============================================================================
  
  program_context_create

  ==========================================================================*/
ProgramContext *program_context_new (void)
  {
  KLOG_IN
  ProgramContext *self = malloc (sizeof (ProgramContext));
  memset (self, 0, sizeof (ProgramContext));
  self->props = kprops_new_empty();
  KLOG_OUT
  return self;
  }

/*============================================================================
  
  program_context_destroy

  ==========================================================================*/
void program_context_destroy (ProgramContext *self)
  {
  KLOG_IN
  if (self)
    {
    if (self->props) kprops_destroy (self->props);
    for (int i = 0; i < self->nonswitch_argc; i++)
      free (self->nonswitch_argv[i]);
    free (self->nonswitch_argv);
    free (self);
    }
  KLOG_OUT
  }

/*==========================================================================

  program_context_get

  ========================================================================*/
char *program_context_get (const ProgramContext *self, 
                   const char *key)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->props != NULL);
  const KString *val = kprops_get_utf8 (self->props, (UTF8 *)key);
  char *ret = NULL;
  if (val) ret = (char *)kstring_to_utf8 (val);
  KLOG_OUT
  return ret;
  }

/*==========================================================================

  program_context_get_boolean

  ========================================================================*/
BOOL program_context_get_boolean (const ProgramContext *self, 
    const char *key, BOOL deflt)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->props != NULL);
  BOOL ret = kprops_get_boolean_utf8 (self->props, (UTF8 *)key, deflt);
  KLOG_OUT
  return ret;
  }

/*==========================================================================

  program_context_get_integer

  ========================================================================*/
BOOL program_context_get_integer (const ProgramContext *self, 
    const char *key, BOOL deflt)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->props != NULL);
  BOOL ret = kprops_get_integer_utf8 (self->props, (UTF8 *)key, deflt);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  program_context_parse_command_line

  ==========================================================================*/
BOOL program_context_parse_command_line (ProgramContext *self, 
        int argc, char **argv)
  {
  KLOG_IN
  BOOL ret = TRUE;
  static struct option long_options[] =
    {
      {"help", no_argument, NULL, 0},
      {"host", required_argument, NULL, 'h'},
      {"log-level", required_argument, NULL, 'l'},
      {"port", required_argument, NULL, 'p'},
      {"version", no_argument, NULL, 'v'},
      {0, 0, 0, 0}
    };

   int opt;
   while (ret)
     {
     int option_index = 0;
     opt = getopt_long (argc, argv, "h:p:vl:",
     long_options, &option_index);

     if (opt == -1) break;

     switch (opt)
       {
       case 0:
         if (strcmp (long_options[option_index].name, "help") == 0)
           program_context_put_boolean (self, "show-usage", TRUE);
         else if (strcmp (long_options[option_index].name, "version") == 0)
           program_context_put_boolean (self, "show-version", TRUE);
         else if (strcmp (long_options[option_index].name, "log-level") == 0)
           program_context_put_integer (self, "log-level", atoi (optarg)); 
         else if (strcmp (long_options[option_index].name, "port") == 0)
           program_context_put_integer (self, "port", atoi (optarg)); 
         else if (strcmp (long_options[option_index].name, "host") == 0)
           program_context_put (self, "host", optarg); 
         else
           exit (-1);
         break;
       case '?': 
         program_context_put_boolean (self, "show-usage", TRUE); break;
       case 'v': 
         program_context_put_boolean (self, "show-version", TRUE); break;
       case 'h': program_context_put (self, "host", optarg); break;
       case 'p': program_context_put_integer (self, "port", 
           atoi (optarg)); break;
       case 'l': program_context_put_integer (self, "log-level", 
           atoi (optarg)); break;
       default:
         ret = FALSE; 
       }
    }

  if (ret)
    {
    self->nonswitch_argc = argc - optind + 1;
    self->nonswitch_argv = malloc (self->nonswitch_argc * sizeof (char *));
    self->nonswitch_argv[0] = strdup (argv[0]);
    int j = 1;
    for (int i = optind; i < argc; i++)
      {
      self->nonswitch_argv[j] = strdup (argv[i]);
      j++;
      }
    }

  if (program_context_get_boolean (self, "show-version", FALSE))
    {
    printf ("%s: %s version %s\n", argv[0], NAME, VERSION);
    printf ("Copyright (c)2020 Kevin Boone\n");
    printf ("Distributed under the terms of the GPL v3.0\n");
    ret = FALSE;
    }

   if (program_context_get_boolean (self, "show-usage", FALSE))
    {
    program_context_show_usage (stdout, argv[0]);
    ret = FALSE;
    }

  KLOG_OUT
  return ret;  
  }

/*============================================================================
  
  program_context_put

  ==========================================================================*/
void program_context_put (ProgramContext *self, const char *name, 
       const char *value)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->props != NULL);
  KString *temp = kstring_new_from_utf8 ((UTF8 *)value);
  kprops_add_utf8 (self->props, (UTF8 *)name, temp);
  kstring_destroy (temp);
  KLOG_OUT
  }

/*============================================================================
  
  program_context_put_boolean

  ==========================================================================*/
void program_context_put_boolean (ProgramContext *self, const char *name, 
       BOOL value)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->props != NULL);
  kprops_put_boolean_utf8 (self->props, (UTF8 *)name, value);
  KLOG_OUT
  }

/*============================================================================
  
  program_context_put_integer

  ==========================================================================*/
void program_context_put_integer (ProgramContext *self, const char *name, 
       int value)
  {
  KLOG_IN
  assert (self != NULL);
  assert (self->props != NULL);
  kprops_put_integer_utf8 (self->props, (UTF8 *)name, value);
  KLOG_OUT
  }

/*============================================================================
  
  program_context_read_rc_file

  ==========================================================================*/
void program_context_read_rc_file (ProgramContext *self, const KPath *path)
  {
  KLOG_IN
  klog_debug (KLOG_CLASS, "Reading RC file %S", 
     kstring_cstr ((KString *)path));

  kprops_from_file (self->props, path);

  KLOG_OUT
  }

/*============================================================================
  
  program_context_read_rc_files

  ==========================================================================*/
void program_context_read_rc_files (ProgramContext *self)
  {
  KLOG_IN
  program_context_read_system_rc_file (self);
  program_context_read_user_rc_file (self);
  KLOG_OUT
  }

/*============================================================================
  
  program_context_read_system_rc_file

  // TODO Windows. 

  ==========================================================================*/
void program_context_read_system_rc_file (ProgramContext *self)
  {
  KLOG_IN
  KPath *path = kpath_new_from_utf8 ((UTF8*) ("/etc/" NAME ".rc"));
  klog_debug (KLOG_CLASS, 
     "System RC file is %S", kstring_cstr ((KString *)path));
  program_context_read_rc_file (self, path);
  kpath_destroy (path);
  KLOG_OUT
  }

/*============================================================================
  
  program_context_read_user_rc_file
  
  // TODO Windows. 

  ==========================================================================*/
void program_context_read_user_rc_file (ProgramContext *self)
  {
  KLOG_IN
  KPath *path = kpath_new_home();
  kpath_append_utf8 (path, (UTF8 *) ("." NAME ".rc"));
  klog_debug (KLOG_CLASS, 
     "User RC file is %S", kstring_cstr ((KString *)path));
  program_context_read_rc_file (self, path);
  kpath_destroy (path);
  KLOG_OUT
  }

/*============================================================================
  
  program_context_show_usage
  
  ==========================================================================*/
void program_context_show_usage (FILE *fout, const char *argv0)
  {
  KLOG_IN
  fprintf (fout, "Usage: %s [options]\n", argv0);
  fprintf (fout, "     --help               show this message\n");
  fprintf (fout, "  -h,--host=[hostname]    bind host or IP\n");
  fprintf (fout, "  -l,--log-level=[0..5]   log level (default 2)\n");
  fprintf (fout, "  -p,--port=[number]      server IP port\n");
  fprintf (fout, "  -v,--version            show version\n");
  KLOG_OUT
  }



