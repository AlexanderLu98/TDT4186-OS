#ifndef ALARMS_H_
#define ALARMS_H_

#include <time.h>

// "It is sufficient if you implement a statically sized array and refuse new entries in the array is full"
#define MAX 10

// "Create a data structure for the alarms"
typedef struct alarm_t
{
    //"The most simple form could be an array of time_t values"
    time_t timestamp;
    //"the PID of the responsible child process or a sound to play"
    unsigned int pid;
} alarm_t;


// Helper method: Get specific alarm with input id
alarm_t get_alarm(unsigned int id);

// Helper method: Get number of alarms
unsigned int get_alarm_count();

// Alarm scheduling
unsigned int schedule_alarm(time_t timestamp);

//Canceling alarms
void cancel_alarm(unsigned int id);

#endif 