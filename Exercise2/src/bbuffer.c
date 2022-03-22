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
  int size;

  // counting semaphores
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
    return NULL;
  }

  // initializing the buffer with INT_MIN to signify an empty cell.
  for(int i=0; i < bb -> size; i++)
  {
    bb -> buffer[i] = INT_MIN;
  }

  // init the semaphores
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
  sem_del(bb-> filled_slots);
  sem_del(bb-> empty_slots);

  free(bb  -> buffer);
  free(&bb -> buffer);
  free(&bb -> size);
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
  V(bb -> filled_slots);
  // do something here
  int item = INT_MIN;
  for(int i = 0; i < bb->size; i++)
  {
    if(bb -> buffer[i] > INT_MIN)
    {
      item = bb -> buffer[i];
      bb -> buffer[i] = INT_MIN;
    }
  }

  // increments the empty semaphore - signaling that there is an open slot in the buffer.
  P(bb -> empty_slots);
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
  // loops through the list checking for empty slots.
  for(int i = 0; i < bb->size; i++)
  {
    if(bb -> buffer[i] == INT_MIN)
    {
      bb -> buffer[i] = fd;
    }
  }
  // updates the number of filled slots - making it possible to read from buffer.
  V(bb -> filled_slots);
}
