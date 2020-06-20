/*============================================================================
  
  klib
  
  klinux_terminal.h

  Definition of the KTerminal class

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/defs.h>
#include <klib/types.h>

struct KLinuxTerminal;
typedef struct _KLinuxTerminal KLinuxTerminal;

BEGIN_DECLS

KLinuxTerminal *klinux_terminal_new (void);

END_DECLS


