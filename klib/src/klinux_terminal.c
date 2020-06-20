/*============================================================================
  
  klib
  
  klinux_terminal.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <klib/klog.h>
#include <klib/kterminal.h>
#include <klib/klinux_terminal.h>

#define TAB_SIZE 8

#define KLOG_CLASS "klib.klinux_terminal"
void klinux_terminal_destroy (KTerminal *self); //FWD
BOOL klinux_terminal_init (KTerminal *self, KString **error); // FWD
BOOL klinux_terminal_deinit (KTerminal *self, KString **error); // FWD
BOOL klinux_terminal_get_size (const KTerminal *self, int *rows, 
      int *cols, KString **error); //FWD
void klinux_terminal_set_raw_mode (KTerminal *self, BOOL raw); //FWD
int klinux_terminal_read_key (const KTerminal *self);
void klinux_terminal_clear (KTerminal *self); // FWD
void klinux_terminal_write_at (const KTerminal *self, int row, 
      int col, const KString *text, BOOL truncate); //FWD
void klinux_terminal_set_cursor (const KTerminal *self, int row, 
      int col); 
void klinux_terminal_set_attributes (const KTerminal *self, int attrs, BOOL on); // FWD
BOOL klinux_terminal_smcup (KTerminal *self, KString **error); //FWD
BOOL klinux_terminal_rmcup (KTerminal *self, KString **error); //FWD
void klinux_terminal_erase_line (KTerminal *self, int line); // FWD

#define TERM_CLEAR "\033[2J\033[1;1H"
#define TERM_ERASE_LINE "\033[K"
#define TERM_CUR_BLOCK "\033[?6c"
#define TERM_CUR_LINE "\033[?2c"
#define TERM_SET_CUR "\033[%d;%dH"
#define TERM_SET_ATTR "\033[%dm"
#define TERM_SMCUP "\033[?1049h"
#define TERM_RMCUP "\033[?1049l"

/*============================================================================
  
  KLinuxTerminal

  ==========================================================================*/
struct _KLinuxTerminal
  {
  KTerminal parent; // Ensure space for inherited fn pointers
  struct termios orig_termios;
  int fd;
  };


/*============================================================================
  
  klinux_terminal_new

  ==========================================================================*/
KLinuxTerminal *klinux_terminal_new (void)
  {
  KLOG_IN
  KLinuxTerminal *self = malloc (sizeof (KLinuxTerminal));
  KTerminal *parent = (KTerminal *) self;
  parent->destroy = klinux_terminal_destroy;
  parent->init = klinux_terminal_init;
  parent->deinit = klinux_terminal_deinit;
  parent->get_size = klinux_terminal_get_size;
  parent->set_raw_mode = klinux_terminal_set_raw_mode;
  parent->read_key = klinux_terminal_read_key;
  parent->clear = klinux_terminal_clear;
  parent->write_at = klinux_terminal_write_at;
  parent->set_cursor = klinux_terminal_set_cursor;
  parent->set_attr = klinux_terminal_set_attributes;
  parent->erase_line = klinux_terminal_erase_line;

  KLOG_OUT
  return self;
  }

/*============================================================================
  
  klinux_terminal_destroy

  ==========================================================================*/
void klinux_terminal_destroy (KTerminal *self)
  {
  if (self)
    {

    free (self);
    }
  }

/*============================================================================
  
  klinux_terminal_deinit

  ==========================================================================*/
BOOL klinux_terminal_deinit (KTerminal *self, KString **error)
  {
  KLOG_IN
  BOOL ret = TRUE; 
  kterminal_clear (self);
  ret = klinux_terminal_rmcup (self, NULL);
  KLOG_OUT;
  return ret;
  }

/*============================================================================
  
  klinux_terminal_erase_line

  ==========================================================================*/
void klinux_terminal_erase_line (KTerminal *self, int line)
  {
  KLOG_IN
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  kterminal_set_cursor (self, line, 0);
  write (_self->fd, TERM_ERASE_LINE, sizeof (TERM_ERASE_LINE) - 1);
  KLOG_OUT;
  }

/*============================================================================
  
  klinux_terminal_init

  ==========================================================================*/
BOOL klinux_terminal_init (KTerminal *self, KString **error)
  {
  KLOG_IN
  BOOL ret = TRUE; 

  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  _self->fd = open ("/dev/tty", O_RDWR);
  if (_self->fd >= 0)
    {
    int rows; int columns;
    // Check that we can get the terminal size. If we can,
    //   everything is probably OK
    if (kterminal_get_size (self, &rows, &columns, error))
      {
      // Nothing else to do here.
      }
    else
      {
      ret = FALSE;
      // error should have been set already
      }
    klinux_terminal_smcup (self, NULL);
    }
  else
    {
    if (error) 
      {
      *error = kstring_new_empty ();
      kstring_append_printf (*error, "Can't open /dev/tty: %s", 
           strerror (errno));
      }
    ret = FALSE;
    }
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  klinux_terminal_clear

  ==========================================================================*/
void klinux_terminal_clear (KTerminal *self)
  {
  KLOG_IN
  assert (self != NULL);
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  write (_self->fd, TERM_CLEAR, sizeof (TERM_CLEAR) - 1);
  write (_self->fd, TERM_CUR_BLOCK, sizeof (TERM_CUR_BLOCK) - 1);
  KLOG_OUT
  }

/*============================================================================
  
  klinux_terminal_get_size

  ==========================================================================*/
BOOL klinux_terminal_get_size (const KTerminal *self, int *rows, 
                                     int *cols, KString **error)
  {
  KLOG_IN
  assert (self != NULL);
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  BOOL ret;
  struct winsize w;
  if (ioctl (_self->fd, TIOCGWINSZ, (unsigned long) &w) == 0)
    {
    *rows = w.ws_row;
    *cols = w.ws_col;
    ret = TRUE;
    }
  else
    {
    if (error)
      {
      *error = kstring_new_empty ();
      kstring_append_printf (*error, "Can't get terminal size: %s", 
           strerror (errno));
      }
    ret = FALSE;
    }
  KLOG_OUT
  return ret;
  }

/*===========================================================================

  klinux_terminal_read_key

===========================================================================*/
int klinux_terminal_read_key (const KTerminal *self)
  {
  int nread;
  char c;
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  while ((nread = read (_self->fd, &c, 1)) != 1)
    {
    if (nread == -1 && errno != EAGAIN) exit (-1); // TODO
    }
  if (c == '\x1b')
    {
    char seq[3];
    if (read (_self->fd, &seq[0], 1) != 1) return '\x1b';
    if (read (_self->fd, &seq[1], 1) != 1) return '\x1b';
    if (seq[0] == '[')
      {
      if (seq[1] >= '0' && seq[1] <= '9')
        {
        if (read(_self->fd, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~')
          {
          switch (seq[1])
            {
            case '3': return VK_DEL; // Usually the key marked "del"
            case '5': return VK_PGUP;
            case '6': return VK_PGDN;
            }
          }
        }
      else
        {
        switch (seq[1])
          {
          case 'A': return VK_UP;
          case 'B': return VK_DOWN;
          case 'C': return VK_RIGHT;
          case 'D': return VK_LEFT;
          case 'H': return VK_HOME;
          case 'F': return VK_END;
          }
        }
      }
    return '\x1b';
    }
  else
    {
    if (c == 127) c = VK_BACK;
    return c;
    }
  }

/*===========================================================================

  klinux_terminal_rmcup

===========================================================================*/
BOOL klinux_terminal_rmcup (KTerminal *self, KString **error)
  {
  KLOG_IN
  assert (self != NULL);
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  write (_self->fd, TERM_RMCUP, sizeof (TERM_SMCUP) - 1);
  KLOG_OUT
  return TRUE;
  }

/*============================================================================
  
  kterminal_set_attr

  ==========================================================================*/
static void klinux_terminal_set_attr (const KTerminal *self, int attr)
  {
  KLOG_IN
  char s[20];
  sprintf (s, TERM_SET_ATTR, attr);
  write (((KLinuxTerminal *)self)->fd, s, strlen (s));
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_set_attributes

  ==========================================================================*/
void klinux_terminal_set_attributes (const KTerminal *self, int attrs, BOOL on)
  {
  KLOG_IN
  assert (self != NULL);

  if (attrs == KTATTR_RESET)
    {
    klinux_terminal_set_attr (self, 0);
    }
  else
    {
    if (attrs & KTATTR_BOLD)
      {
      klinux_terminal_set_attr (self, on ? 1 : 21);
      }
    if (attrs & KTATTR_ITALIC)
      {
      klinux_terminal_set_attr (self, on ? 3 : 23);
      }
    if (attrs & KTATTR_REVERSE)
      {
      klinux_terminal_set_attr (self, on ? 7 : 27);
      }
    }

  KLOG_OUT
  }

/*============================================================================
  
  klinux_terminal_set_cursor

  ==========================================================================*/
void klinux_terminal_set_cursor (const KTerminal *self, int row, 
      int col)
  {
  KLOG_IN
  char s[40];
  sprintf (s, TERM_SET_CUR, row + 1, col + 1);
  write (((KLinuxTerminal *)self)->fd, s, strlen (s));
  KLOG_OUT
  }


/*============================================================================
  
  klinux_terminal_set_raw_mode

  ==========================================================================*/
void klinux_terminal_set_raw_mode (KTerminal *self, BOOL raw)
  {
  KLOG_IN
  assert (self != NULL);
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  klog_debug (KLOG_CLASS, "Set raw mode: %d", raw);

  if (raw)
    {
    tcgetattr (_self->fd, &_self->orig_termios);
    struct termios raw = _self->orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VTIME] = 1;
    raw.c_cc[VMIN] = 0;
    tcsetattr (_self->fd, TCSAFLUSH, &raw);
    }
  else
    {
    klog_warn (KLOG_CLASS,
      "Called set_raw_mode (FALSE) with no previous set_raw_mode (TRUE)");
    tcsetattr(_self->fd, TCSAFLUSH, &_self->orig_termios);
    }

  KLOG_OUT
  }

/*===========================================================================

  klinux_terminal_smcup

===========================================================================*/
BOOL klinux_terminal_smcup (KTerminal *self, KString **error)
  {
  KLOG_IN
  assert (self != NULL);
  KLinuxTerminal *_self = (KLinuxTerminal *)self;
  write (_self->fd, TERM_SMCUP, sizeof (TERM_SMCUP) - 1);
  KLOG_OUT
  return TRUE;
  }

/*===========================================================================

  klinux_terminal_truncate_line

===========================================================================*/
/*
static void klinux_terminal_truncate_line (int columns, char *line, int *len)
  {
  int dlen = 0;
  char *p = line;
  *len = 0;

  while (*p && dlen < columns)
    {
    if (*p == '\t')
      {
      // TODO -- this logic only works with 8-space tabs
      dlen += TAB_SIZE;
      dlen &= 0xFFFFFFF8;
      }
    else       
      dlen++;
    p++;
    (*len)++;
    }
  *p = 0;
  }
i*/

/*===========================================================================

  klinux_terminal_write_at

===========================================================================*/
void klinux_terminal_write_at (const KTerminal *self, int row, 
      int col, const KString *text, BOOL truncate)
  {
  KLOG_IN
  KLinuxTerminal *_self = (KLinuxTerminal*)self;
  int len = kstring_length (text); 
  int rows = 24; int columns = 80; // defaults, in case get_size fails
  klinux_terminal_set_cursor (self, row, col);
  klinux_terminal_get_size (self, &rows, &columns, NULL);

  int newcol = col;
  for (int i = 0; i < len && newcol < columns; i++)
    {
    // TODO non-print characters
    UTF8 u[UTF8_MAX_BYTES + 1];
    int n = kstring_get_utf8 (text, i, u);
    write (_self->fd, u, n); 
    newcol++;
    }
  KLOG_OUT
  }



