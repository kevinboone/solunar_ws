/*============================================================================
  
  klib
  
  klist.c

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <klib/klog.h>
#include <klib/klist.h>

#define KLOG_CLASS "klib.klist"

/*============================================================================
  
  ListItem

  ==========================================================================*/
typedef struct _ListItem
  {
  struct _ListItem *next;
  void *data;
  } ListItem;


/*============================================================================
  
  KList

  ==========================================================================*/
struct _KList
  {
  KListFreeFn free_fn;
  ListItem *head;
  size_t length;
  };


/*============================================================================
  
  klist_new_empty

  ==========================================================================*/
extern KList *klist_new_empty (KListFreeFn free_fn)
  {
  KLOG_IN
  KList *self = malloc (sizeof (KList));
  self->free_fn = free_fn;
  self->length = 0;
  self->head = NULL;
  KLOG_OUT
  return self;
  }
  
/*============================================================================
  
  klist_destroy

  ==========================================================================*/
void klist_destroy (KList *self)
  {
  KLOG_IN
  if (self)
    {
    klist_clear (self);
    free (self);
    }
  KLOG_OUT
  }

/*============================================================================
  
  klist_append

  ==========================================================================*/
void klist_append (KList *self, void *ref)
  {
  KLOG_IN
  assert (self != NULL);
  assert (ref != NULL);

  ListItem *i = malloc (sizeof (ListItem));
  i->data = ref;
  i->next = NULL;

  if (self->head)
    {
    ListItem *l = self->head;
    while (l->next)
      l = l->next;
    l->next = i;
    }
  else
    {
    self->head = i;
    }

  self->length++;
  KLOG_OUT
  }

/*============================================================================
  
  klist_clear

  ==========================================================================*/
void klist_clear (KList *self)
  {
  KLOG_IN
  assert (self != NULL);

  ListItem *l = self->head;
  while (l)
    {
    // It is legitimate for free_fn to be NULL
    if (self->free_fn) self->free_fn (l->data);
    ListItem *temp = l;
    l = l->next;
    free (temp);
    }
  
  self->length = 0;
  KLOG_OUT
  }

/*============================================================================
  
  klist_get

  ==========================================================================*/
void *klist_get (const KList *self, size_t index)
  {
  KLOG_IN
  assert (self != 0);
  assert (index >= 0);
  assert (index < self->length);
  ListItem *l = self->head;
  int i = 0;
  while (l != NULL && i != index)
    {
    l = l->next;
    i++;
    }
  KLOG_OUT
  return l->data;
  }

/*============================================================================
  
  klist_length

  ==========================================================================*/
size_t klist_length (const KList *self)
  {
  KLOG_IN
  assert (self != NULL);
  return self->length;
  KLOG_OUT
  }

/*============================================================================
  
  klist_remove

  ==========================================================================*/
void klist_remove (KList *self, const void *item, ListCompareFn fn)
  {
  KLOG_IN
  assert (self != NULL);
  assert (item != NULL);
  assert (fn != NULL);
  ListItem *l = self->head;
  ListItem *last_good = NULL;
  while (l != NULL)
    {
    if (fn (l->data, item, NULL) == 0)
      {
      if (l == self->head)
        {
        self->head = l->next; // l-> next might be null
        }
      else
        {
        if (last_good) last_good->next = l->next;
        }
      if (self->free_fn) self->free_fn (l->data);
      ListItem *temp = l->next;
      free (l);
      self->length--;
      l = temp;
      }
    else
      {
      last_good = l;
      l = l->next;
      }
    }
  KLOG_OUT                        
  }

/*============================================================================
  
  klist_remove_ref

  ==========================================================================*/
void klist_remove_ref (KList *self, const void *ref, BOOL destroy)
  {
  KLOG_IN
  assert (self != NULL);
  ListItem *l = self->head;
  ListItem *last_good = NULL;
  while (l != NULL)
    {
    if (l->data == ref)
      {
      if (l == self->head)
        {
        self->head = l->next; // l-> next might be null
        }
      else
        {
        if (last_good) last_good->next = l->next;
        }
      if (destroy && self->free_fn) self->free_fn (l->data);
      ListItem *temp = l->next;
      self->length--;
      free (l);
      l = temp;
      }
    else
      {
      last_good = l;
      l = l->next;
      }
    }

  KLOG_OUT;
  }

/*============================================================================
  
  klist_sort

  ==========================================================================*/
#ifdef __GLIBC__
void klist_sort (KList *self, ListSortFn fn, void *user_data)
  {
  KLOG_IN
  int length = klist_length (self);
  
  void **temp = malloc (length * sizeof (void *));
  ListItem *l = self->head;
  int i = 0;
  while (l != NULL)
    {
    temp[i] = l->data;
    l = l->next;
    i++;
    }

  // Sort temp

  qsort_r (temp, length, sizeof (void *), fn, user_data); 
  
  // Copy the sorted data back
   
  l = self->head;
  i = 0;
  while (l != NULL)
    {
    l->data = temp[i];
    l = l->next;
    i++;
    }
  
  free (temp);

  KLOG_OUT
  }
#endif


/*============================================================================
  
  klist_transfer_list

  ==========================================================================*/
void klist_transfer_list (KList *self, KList *list)
  {
  KLOG_IN
  assert (self != NULL);
  assert (list != NULL);
  for (int i = list->length - 1; i >= 0; i--)
    {
    void *ref = klist_get (list, i);
    klist_append (self, ref);
    klist_remove_ref (list, ref, FALSE); // Don't destroy -- item has moved 
    }
  KLOG_OUT
  }


