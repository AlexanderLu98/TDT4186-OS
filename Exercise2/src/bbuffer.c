#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "bbuffer.h"
#include "sem.h"

/*
 * Bounded Buffer implementation to manage int values that supports multiple
 * readers and writers.
 *
 * The bbuffer module uses the sem module API to synchronize concurrent access
 * of readers and writers to the bounded buffer.
 */

/* Opaque type of a Bounded Buffer.
 * ...you need to figure out the contents of struct BNDBUF yourself
 */

typedef struct BNDBUF
{
  // buffer of ints and it's length
  int *buffer;
  unsigned int size;
  int *old;
  int *new;

  // counting semaphores
  SEM *lock;
  SEM *filled_slots;
  SEM *empty_slots;

}BNDBUF;

/* Creates a new Bounded Buffer.
 *
 * This function creates a new bounded buffer and all the helper data
 * structures required by the buffer, including semaphores for
 * synchronization. If an error occurs during the initialization the
 * implementation shall free all resources already allocated by then.
 *
 * Parameters:
 *
 * size     The number of integers that can be stored in the bounded buffer.
 *
 * Returns:
 *
 * handle for the created bounded buffer, or NULL if an error occured.
 */

BNDBUF *bb_init(unsigned int size)
{
  BNDBUF *bb = malloc(sizeof(BNDBUF));
  if(!bb)
  {
    return NULL;
  }

  // setting buffer size
  bb -> size = size;

  bb -> buffer = malloc(bb->size*sizeof(int));
  if(!(&bb-> buffer))
  {
    free(bb);
    printf("OH NO BUFFER! \n");
    return NULL;
  }

  // setting both new and old to the start adress of the buffer
  bb -> new = bb -> buffer;
  bb -> old = bb -> buffer;

  // init the semaphores
  bb -> lock         = sem_init(1);
  bb -> filled_slots = sem_init(0);
  bb -> empty_slots  = sem_init(size);

  //printf("filled_slots %d: \n", bb -> filled_slots -> value);
  //printf("empty_slots %d: \n",  bb -> empty_slots  -> value);

  return bb;
}

/* Destroys a Bounded Buffer.
 *
 * All resources associated with the bounded buffer are released.
 *
 * Parameters:
 *
 * bb       Handle of the bounded buffer that shall be freed.
 */

void bb_del(BNDBUF *bb)
{
  sem_del(bb-> lock);
  sem_del(bb-> filled_slots);
  sem_del(bb-> empty_slots);

  free(bb -> buffer);
  free(bb -> new);
  free(bb -> old);
  free(bb);
}

/* Retrieve an element from the bounded buffer.
 *
 * This function removes an element from the bounded buffer. If the bounded
 * buffer is empty, the function blocks until an element is added to the
 * buffer.
 *
 * Parameters:
 *
 * bb         Handle of the bounded buffer.
 *
 * Returns:
 *
 * the int element
 */

int  bb_get(BNDBUF *bb)
{
  // checks if there is any data if so.
  P(bb -> filled_slots);
  P(bb -> lock);
  int item = *bb->old;
  bb -> old += sizeof(int);
  if (bb -> old == bb -> buffer + bb -> size*sizeof(int))
      bb -> old = bb -> buffer;
  V(bb -> lock);
  V(bb -> empty_slots);
  return item;
}

/* Add an element to the bounded buffer.
 *
 * This function adds an element to the bounded buffer. If the bounded
 * buffer is full, the function blocks until an element is removed from
 * the buffer.
 *
 * Parameters:
 *
 * bb     Handle of the bounded buffer.
 * fd     Value that shall be added to the buffer.
 *
 * Returns:
 *
 * the int element
 */

void bb_add(BNDBUF *bb, int fd)
{
  // tries to put something in to the buffer, blocks if there are no empty slots
  P(bb -> empty_slots);
  P(bb -> lock);
  *bb->new = fd;
  bb->new += sizeof(int);
  if (bb->new == bb->buffer + bb->size*sizeof(int))
    bb->new = bb->buffer;
  V(bb -> lock);
  V(bb -> filled_slots);
}
