/*============================================================================
  
  klib
  
  kterminal.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <klib/klog.h>
#include <klib/kterminal.h>
#include <klib/klinux_terminal.h>

#define KLOG_CLASS "klib.kterminal"


/*============================================================================
  
  kterminal_clear

  ==========================================================================*/
void kterminal_clear (KTerminal *self)
  {
  KLOG_IN
  assert (self != NULL);
  self->clear (self);
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_destroy

  ==========================================================================*/
void kterminal_destroy (KTerminal *self)
  {
  KLOG_IN
  if (self)
    self->destroy (self);
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_deinit

  ==========================================================================*/
BOOL kterminal_deinit (KTerminal *self, KString **error)
  {
  KLOG_IN
  assert (self != NULL);
  BOOL ret = self->deinit (self, error);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kterminal_erase_line

  ==========================================================================*/
void kterminal_erase_line (KTerminal *self, int line)
  {
  KLOG_IN
  assert (self != NULL);
  self->erase_line (self, line);
  KLOG_OUT;
  }

/*============================================================================
  
  kterminal_init

  ==========================================================================*/
BOOL kterminal_init (KTerminal *self, KString **error)
  {
  KLOG_IN
  assert (self != NULL);
  BOOL ret = self->init (self, error);
  KLOG_OUT
  return ret;
  }


/*============================================================================
  
  kterminal_get_size

  ==========================================================================*/
BOOL kterminal_get_size (const KTerminal *self, int *rows, 
                                     int *cols, KString **error)
  {
  KLOG_IN
  assert (self != NULL);
  BOOL ret = self->get_size (self, rows, cols, error);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kterminal_read_key

  ==========================================================================*/
int kterminal_read_key (const KTerminal *self)
  {
  KLOG_IN
  assert (self != NULL);
  int ret = self->read_key (self);
  KLOG_OUT
  return ret;
  }

/*============================================================================
  
  kterminal_set_attributes

  ==========================================================================*/
void kterminal_set_attributes (const KTerminal *self, int attrs, BOOL on)
  {
  KLOG_IN
  assert (self != NULL);
  self->set_attr (self, attrs, on);
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_set_cursor

  ==========================================================================*/
void kterminal_set_cursor (const KTerminal *self, int row, 
      int col)
  {
  KLOG_IN
  assert (self != NULL);
  self->set_cursor (self, row, col);
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_set_raw_mode

  ==========================================================================*/
void kterminal_set_raw_mode (KTerminal *self, BOOL raw)
  {
  KLOG_IN
  assert (self != NULL);
  self->set_raw_mode (self, raw);
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_write_line

  ==========================================================================*/
void kterminal_write_at (const KTerminal *self, int row, int col, 
      const KString *text, BOOL truncate)
  {
  KLOG_IN
  assert (self != NULL);
  self->write_at (self, row, col, text, truncate);
  KLOG_OUT
  }

/*============================================================================
  
  kterminal_write_line

  ==========================================================================*/
void kterminal_write_at_utf8 (const KTerminal *self, int row, int col, 
      const UTF8 *text, BOOL truncate)
  {
  KLOG_IN
  assert (self != NULL);
  assert (text != NULL);
  KString *temp = kstring_new_from_utf8 (text);
  self->write_at (self, row, col, temp, truncate);
  kstring_destroy (temp);
  KLOG_OUT
  }





