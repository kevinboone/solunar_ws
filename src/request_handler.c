/*============================================================================

  solunar_ws 

  request_handler.c

  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

============================================================================*/

#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <wchar.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <microhttpd.h>
#include <klib/klib.h>
#include <libsolunar/libsolunar.h>
#include "request_handler.h" 

#define KLOG_CLASS "solunar_ws.request_handler"

struct _RequestHandler
  {
  BOOL shutdown_requested;
  int requests;
  int ok_requests;
  const ProgramContext *context;
  }; 

typedef void (*APIHandlerFn) (const RequestHandler *self, 
      const KList *list, char **response, int *code);

typedef struct _APIHandler 
  {
  const char *name;
  APIHandlerFn fn;
  } APIHandler;

void request_handler_day (const RequestHandler *self, const KList *list, 
      char **response, int *code);
void request_handler_health (const RequestHandler *self, const KList *list, 
      char **response, int *code);
void request_handler_metrics (const RequestHandler *self, const KList *list, 
      char **response, int *code);

APIHandler handlers[] = 
  {
  {"day", request_handler_day},
  {"health", request_handler_health},
  {"metrics", request_handler_metrics},
  {NULL, NULL}
  };

/*============================================================================

  request_handler_create

============================================================================*/
RequestHandler *request_handler_create (const ProgramContext *context)
  {
  KLOG_IN
  RequestHandler *self = malloc (sizeof (RequestHandler)); 
  self->shutdown_requested = FALSE;
  self->context = context;
  self->requests = 0;
  self->ok_requests = 0;
  KLOG_OUT 
  return self;
  }

/*============================================================================

  request_handler_destroy

============================================================================*/
void request_handler_destroy (RequestHandler *self)
  {
  KLOG_IN
  if (self)
    {
    free (self);
    }
  KLOG_OUT 
  }


/*============================================================================

  request_handler_day

  Genarate a request for the /day/city/date API

============================================================================*/
void request_handler_day (const RequestHandler *self, const KList *args, 
       char **response, int *code)
  {
  KLOG_IN
  int argc = klist_length ((KList *)args);
  if (argc == 3)
    {
    const char *city = klist_get ((KList *)args, 1); 
    const char *date = klist_get ((KList *)args, 2); 

    klog_debug (KLOG_CLASS, "/day invoked with city=%s and date=%s", 
       city, date); 

    time_t t_date = datetimeconv_parse_date (date, 2, 0, NULL);
    if (t_date)
      {
      KList *city_list = solcity_find_matching ((UTF8 *)city);
      if (city_list)
        {
        int cities = klist_length (city_list);
        if (cities == 1)
	  {
	  const SolCity *c = klist_get (city_list, 0);
	  const char *tz_city = solcity_get_tz_name (c);
	  const char *full_city = solcity_get_name (c);
	  double latitude = solcity_get_latitude (c);
	  double longitude = solcity_get_longitude (c);
          SolunarDaySummary *sds = solunar_day_summary_create 
            (t_date, latitude, longitude, full_city, tz_city);

          KString *json = solunar_day_summary_to_json (sds);
          *response = (char *)kstring_to_utf8 (json);
          kstring_destroy (json);

          solunar_day_summary_destroy (sds);
          *code = 200;
	  }
	else
	  {
	  asprintf (response, "Ambiguous city: %d matches\n", cities);
          *code = 400;
	  }
        klist_destroy (city_list);
        }
      else
        {
        asprintf (response, "Could not find city\n");
        *code = 400;
        }

      }
    else
      {
      asprintf (response, "Could not parse date\n");
      *code = 400;
      }
    }
  else
    {
    asprintf (response, "/day API takes two arguments -- city and date\n");
    *code = 400;
    }
  KLOG_OUT
  }


/*============================================================================

  request_handler_health

  Genarate a request for the /health API

============================================================================*/
void request_handler_health (const RequestHandler *self, const KList *args, 
    char **response, int *code) 
  {
  asprintf (response, "{\"health\": \"OK\"}\n");
  *code = 200;
  }


/*============================================================================

  request_handler_metrics

  Genarate a request for the /metrics API

============================================================================*/

void request_handler_metrics (const RequestHandler *self, const KList *args, 
    char **response, int *code)
  {
  asprintf (response, "{\"requests\": %d,\"requests_ok\": %d,\"requests_error\": %d}\n", 
    self->requests, self->ok_requests, self->requests - self->ok_requests);
  *code = 200;
  }


/*============================================================================

  request_handler_api

============================================================================*/
void request_handler_api (RequestHandler *self, const char *_uri, 
       const KProps* arguments, int *code, char **page)
  {
  KLOG_IN
  klog_debug (KLOG_CLASS, "API request: %s", _uri);

  char *uri = strdup (_uri);
  KList *args = klist_new_empty (free);
  char *sp = NULL;
  char *arg = strtok_r (uri, "/", &sp);
  while (arg)
    {
    klist_append (args, strdup (arg));
    arg = strtok_r (NULL, "/", &sp);
    }

  int argc = klist_length (args);
  if (argc > 0)
    {
    APIHandler *ah = &handlers[0];
    int i = 0;
    BOOL done = FALSE;
    while (ah->name && !done)
      {
      const char *arg0 = klist_get (args, 0);
      if (strcmp (arg0, ah->name) == 0)
        {
        ah->fn (self, args, page, code); 
        done = TRUE;
        }
      i++;
      ah = &handlers[i];
      }
    if (!done)
      {
      // Error no match
      asprintf (page, "Not found\n");
      *code = 404;
      }
    }
  else
    {
    // Error no args
    asprintf (page, "Not found\n");
    *code = 404;
    }

  KLOG_OUT

  klist_destroy (args);
  if (*code == 200)
    self->ok_requests++;
  self->requests++;

  free (uri);
  }

/*============================================================================

  request_handler_shutdown_requested

============================================================================*/
BOOL request_handler_shutdown_requested (const RequestHandler *self) 
  {
  KLOG_IN
  BOOL ret = self->shutdown_requested;
  KLOG_OUT
  return ret;
  }

/*============================================================================

  request_handler_request_shutdown

============================================================================*/
void request_handler_request_shutdown (RequestHandler *self)
  {
  KLOG_IN
  klog_info (KLOG_CLASS, "Shutdown requested");
  self->shutdown_requested = TRUE;
  KLOG_OUT
  }


/*============================================================================

  request_handler_get_program_context

============================================================================*/
const ProgramContext *request_handler_get_program_context 
      (const RequestHandler *self)
  {
  KLOG_IN
  const ProgramContext *ret = self->context;
  KLOG_OUT
  return ret;
  }

