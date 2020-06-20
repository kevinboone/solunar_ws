/*============================================================================

  hello-ws-c
  request_handler.h
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <klib/klib.h> 
#include "program_context.h"

struct _RequestHandler;
typedef struct _RequestHandler RequestHandler;

BEGIN_DECLS

RequestHandler *request_handler_create (const ProgramContext *content);

void            request_handler_destroy (RequestHandler *self);

void request_handler_api (RequestHandler *self, const char *uri, 
      const KProps *arguments, int *code, char **buff);

BOOL request_handler_shutdown_requested (const RequestHandler *self);

void request_handler_request_shutdown (RequestHandler *self);

const ProgramContext *request_handler_get_program_context 
        (const RequestHandler *self);

END_DECLS


