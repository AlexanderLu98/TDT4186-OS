#include "sem.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
  A semaphore is defined as “a non-negative integer number” with
  two atomic operations

  P:(from Dutch “prolaag” = “decrement”; also down or wait)
     • if the semaphore has the value 0, the process calling P is blocked
     • otherwise, the semaphore value is decremented
  V:(from Dutch “verhoog” = “increment”; also up or signal)
     • a process waiting for the semaphore (due to a previous call to P) is unblocked
     • otherwise, the semaphore is incremented by 1

  Semaphore implementation for the synchronization of POSIX threads.

  This module implements counting P/V semaphores suitable for the
  synchronization of POSIX threads. POSIX mutexes and condition variables
  shall be utilized to implement the semaphore operations.

  Opaque type of a semaphore.
  ...you need to figure out the contents of struct SEM yourself!
*/

typedef struct SEM
{
  int value;
  pthread_mutex_t mutex;
  pthread_cond_t  conditon_var;
}SEM;


/* Creates a new semaphore.
 *
 * This function creates a new semaphore. If an error occurs during the
 * initialization, the implementation shall free all resources already
 * allocated so far.
 *
 * Parameters:
 *
 * initVal      the initial value of the semaphore
 *
 * Returns:
 *
 * handle for the created semaphore, or NULL if an error occured.
 */

SEM *sem_init(int initVal)
{
  SEM *sem = malloc(sizeof(SEM));
  if(!sem)
  {
    return NULL;
  }

  sem -> value = initVal;

  if(!pthread_mutex_init(&sem -> mutex, NULL))
  {
    free(sem);
    return NULL;
  }
  if(!pthread_cond_init(&sem -> conditon_var, NULL))
  {
    pthread_mutex_destroy(&sem -> mutex);
    free(sem);
    return NULL;
  }

  return sem;
}

/* Destroys a semaphore and frees all associated resources.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to destroy
 *
 * Returns:
 *
 * 0 on success, negative value on error.
 *
 * In case of an error, not all resources may have been freed, but
 * nevertheless the semaphore handle must not be used any more.
 */

int sem_del(SEM *sem)
{
  int sucess = 1;
  sucess = pthread_mutex_destroy(&sem -> mutex);
  sucess = pthread_cond_destroy(&sem -> conditon_var);
  free(&sem -> value);
  free(sem);
  return sucess? 0 : -1;
}

/* P (wait) operation.
 *
 * Attempts to decrement the semaphore value by 1. If the semaphore value
 * is 0, the operation blocks until a V operation increments the value and
 * the P operation succeeds.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to decrement
 */

void P(SEM *sem)
{
  pthread_mutex_lock(&sem -> mutex);
  while(sem -> value == 0){pthread_cond_wait(&sem->conditon_var, &sem->mutex);}
  sem -> value--;
  pthread_mutex_unlock(&sem -> mutex);
}

/* V (signal) operation.
 *
 * Increments the semaphore value by 1 and notifies P operations that are
 * blocked on the semaphore of the change.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to increment
 */

void V(SEM *sem)
{
  pthread_mutex_lock(&sem-> mutex);
  sem -> value ++;
  if(sem -> value > 0){pthread_cond_signal(&sem -> conditon_var);}
  pthread_mutex_unlock(&sem-> mutex);
}
