/*============================================================================
  
  klib
  
  kterminal.h

  Definition of the KTerminal class

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include <klib/defs.h>
#include <klib/types.h>
#include <klib/kstring.h>

// Key codes for cursor movement, etc
#define VK_BACK     8
#define VK_TAB      9
#define VK_ENTER   10
// Note that Linux console/terminal usually sends DEL when 
//   backspace is pressed
#define VK_DEL    127 
#define VK_DOWN  1000
#define VK_UP    1001 
#define VK_LEFT  1002 
#define VK_RIGHT 1003 
#define VK_PGUP  1004 
#define VK_PGDN  1005 
#define VK_HOME  1006 
#define VK_END   10067

//Terminal attributes
#define KTATTR_REVERSE   0x0001
#define KTATTR_ITALIC    0x0002
#define KTATTR_BOLD      0x0004
#define KTATTR_RESET     0x8000

#define KTATTR_OFF      0
#define KTATTR_ON	1

struct _KTerminal;

typedef void (*KTerminalDestroyFn) (struct _KTerminal *self);
typedef BOOL (*KTerminalInitFn) (struct _KTerminal *self, KString **error);
typedef BOOL (*KTerminalGetSizeFn) (const struct _KTerminal *self, int *rows, 
                    int *cols, KString **error);
typedef void (*KTerminalSetRawModeFn) (struct _KTerminal *self, BOOL raw);
typedef int (*KTerminalReadKeyFn) (const struct _KTerminal *self);
typedef void (*KTerminalClearFn) (struct _KTerminal *self);
typedef void (*KTerminalSetCursorFn) (const struct _KTerminal *self,
                    int row, int col);
typedef void (*KTerminalWriteAtFn) (const struct _KTerminal *self, int row, 
                    int col, const KString *text, BOOL truncate);
typedef void (*KTerminalSetAttrFn) (const struct _KTerminal *self, int attrs, BOOL on);
typedef void (*KTerminalEraseLineFn) (struct _KTerminal *self, int line);

typedef struct _KTerminal
  {
  KTerminalInitFn init;
  KTerminalInitFn deinit;
  KTerminalDestroyFn destroy;
  KTerminalGetSizeFn get_size;
  KTerminalSetRawModeFn set_raw_mode;
  KTerminalReadKeyFn read_key;
  KTerminalClearFn clear;
  KTerminalWriteAtFn write_at;
  KTerminalSetCursorFn set_cursor;
  KTerminalSetAttrFn set_attr;
  KTerminalEraseLineFn erase_line;
  } KTerminal;


BEGIN_DECLS

extern void    kterminal_destroy (KTerminal *self);

extern BOOL    kterminal_deinit (KTerminal *self, KString **error);

extern void    kterminal_clear (KTerminal *self);

extern BOOL    kterminal_get_size (const KTerminal *self, int *rows, 
                   int *cols, KString **error);

// Line is zero-based
extern void    kterminal_erase_line (KTerminal *self, int line);

extern BOOL    kterminal_init (KTerminal *self, KString **error);

extern int     kterminal_read_key (const KTerminal *self);

extern void    kterminal_set_attributes (const KTerminal *self, int attrs, BOOL on);

extern void    kterminal_set_cursor (const KTerminal *self, int row, 
                   int col);

extern void    kterminal_set_raw_mode (KTerminal *self, BOOL raw);

extern void    kterminal_write_at (const KTerminal *self, int row, 
                   int col, const KString *text, BOOL truncate);
extern void    kterminal_write_at_utf8 (const KTerminal *self, int row, 
                   int col, const UTF8 *text, BOOL truncate);

END_DECLS

