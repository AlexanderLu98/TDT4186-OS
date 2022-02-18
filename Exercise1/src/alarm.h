#ifdef _WIN32
#include <Windows.h>
#define SYSTYPE 3
#elif __linux__
#include <unistd.h>
#define SYSTYPE 2
#elif __APPLE__
#include <unistd.h>
#define SYSTYPE 1
#else
#define SYSTYPE 0
#endif

#define __USE_XOPEN
#define _GNU_SOURCE
#define _XOPEN_SOURCE

#ifndef ALARMS_H_
#define ALARMS_H_
#endif

// "It is sufficient if you implement a statically sized array and refuse new entries in the array is full"
#define MAX 3

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>




// "Create a data structure for the alarms"
typedef struct alarm_t
{
    //"The most simple form could be an array of time_t values"
    time_t timestamp;
    //"the PID of the responsible child process or a sound to play"
    unsigned int pid;
    // setting an alarm as active or inactive, 0 is default and inactive, 1 is active.
    unsigned int active;
} alarm_t;

// Helper method: Find the empty spaces in the alarm array
int get_free_alarm_id();

// Helper method: Get specific alarm with input id
alarm_t get_alarm(unsigned int id);

// Helper method: Get number of alarms
unsigned int get_alarm_count();

// Alarm scheduling
unsigned int schedule_alarm(time_t timestamp);

//Canceling alarms
void cancel_alarm(unsigned int id);

//list active alarms
void list_active_alarms();

// playing a custom sound for the alarm
void play_alarm_sound();
