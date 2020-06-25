/*============================================================================
  
  solunar_ws 
  
  main.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#include <stdio.h> 
#include <time.h> 
#include <stdlib.h> 
#include <string.h> 
#include <signal.h> 
#include <unistd.h> 
#include <microhttpd.h> 
#include <klib/klib.h> 
#include <libsolunar/libsolunar.h> 
#include "program_context.h" 
#include "request_handler.h" 

#define KLOG_CLASS "solunar_ws.main"

/*============================================================================

  header_iterator

============================================================================*/
static int header_iterator (void *data, enum MHD_ValueKind kind, 
        const char *key, const char *value)
  {
  KLOG_IN
  KProps *headers = (KProps *) data;
  KString *temp = kstring_new_from_utf8 ((UTF8 *)value);
  kprops_add_utf8 (headers, (UTF8 *)key, temp);
  kstring_destroy (temp);
  KLOG_OUT
  return MHD_YES;
  }

/*============================================================================

  argument_iterator

============================================================================*/
static int argument_iterator (void *data, enum MHD_ValueKind kind, 
        const char *key, const char *value)
  {
  KLOG_IN
  KProps *headers = (KProps *) data;
  if (value) // Avoid trying to store a null header value
    {
    KString *temp = kstring_new_from_utf8 ((UTF8 *)value);
    kprops_add_utf8 (headers, (UTF8 *)key, temp);
    kstring_destroy (temp);
    }
  KLOG_OUT
  return MHD_YES;
  }

/*============================================================================

  handle_request 

  This is the request handler passed to microhttpd. It does the low-level
  stuff that microhttpd needs, but all the real processing is delegated
  to the RequestHandler class.

============================================================================*/
int handle_request (void *_request_handler, 
      struct MHD_Connection *connection, const char *url,
      const char *method, const char *version, const char *upload_data,
      size_t *upload_data_size, void **con_cls)
  {
  KLOG_IN
  int ret = MHD_YES;
  RequestHandler *request_handler = (RequestHandler*) _request_handler;

  klog_debug (KLOG_CLASS, "request: %s", url);

  KProps *headers = kprops_new_empty();
  MHD_get_connection_values (connection, MHD_HEADER_KIND, 
       header_iterator, headers);

  KProps *arguments = kprops_new_empty();
  MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, 
       argument_iterator, arguments);

  struct MHD_Response *response;

  int code = 200;
  char *buff;
  request_handler_api (request_handler, url, arguments, &code, &buff);

  response = MHD_create_response_from_buffer (strlen (buff),
           (void*) buff, MHD_RESPMEM_MUST_FREE);
  if (code == 200)
    MHD_add_response_header (response, "Content-Type", 
            "application/json; charset=utf8");
  else
    MHD_add_response_header (response, "Content-Type", 
            "text/plain; charset=utf8");

  MHD_add_response_header (response, "Cache-Control", "no-cache");
  ret = MHD_queue_response (connection, code, response);
  MHD_destroy_response (response);

  kprops_destroy (headers);
  kprops_destroy (arguments);
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  main 

  ==========================================================================*/
int main (int argc, char **argv)
  {
  ProgramContext *context = program_context_new();

  // Set the initial log level from an env var, for OpenShift/Kubernetes
  //  compatibility
  char *ll_env = getenv ("SOLUNAR_WS_LOG_LEVEL");
  if (ll_env)
    klog_set_log_level (atoi(ll_env));
  else
    klog_set_log_level (KLOG_INFO);
 
  program_context_read_rc_files (context);
  if (program_context_parse_command_line (context, argc, argv))
    {
    int log_level = 
      program_context_get_integer (context, "log-level", KLOG_INFO);
    klog_set_log_level (log_level);

    char *host = program_context_get (context, "host");
    if (!host) host = strdup ("0.0.0.0");
    int port = program_context_get_integer (context, "port", 
         8080);

    klog_info (KLOG_CLASS, "host=%s, port=%d", host, port);

    RequestHandler *request_handler = request_handler_create (context);

    sigset_t base_mask, waiting_mask;

    sigemptyset (&base_mask);
    sigaddset (&base_mask, SIGINT);
    sigaddset (&base_mask, SIGTSTP);
    sigaddset (&base_mask, SIGHUP);
    sigaddset (&base_mask, SIGQUIT);
    sigprocmask (SIG_SETMASK, &base_mask, NULL);

    klog_info (KLOG_CLASS, "HTTP server starting");

    struct MHD_Daemon *daemon = MHD_start_daemon 
	  (MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL,
	   handle_request, request_handler, MHD_OPTION_END);

    if (daemon)
      {
      while (!request_handler_shutdown_requested (request_handler))
	 {
	 usleep (1000000);
	 sigpending (&waiting_mask);
	 if (sigismember (&waiting_mask, SIGINT) ||
	     sigismember (&waiting_mask, SIGTSTP) ||
	     sigismember (&waiting_mask, SIGQUIT) ||
	     sigismember (&waiting_mask, SIGHUP))
	   {
	   klog_warn (KLOG_CLASS, "Shutting down on signal");
	   request_handler_request_shutdown (request_handler);
	   }
	 }

      klog_info (KLOG_CLASS, "HTTP server stopping");

      MHD_stop_daemon (daemon);
      }
    else
     {
     klog_error (KLOG_CLASS, 
       "Can't start HTTP server (check port %d is not in use)", port);
     }

    request_handler_destroy (request_handler);

    free (host);
    }

  program_context_destroy (context);
  }



