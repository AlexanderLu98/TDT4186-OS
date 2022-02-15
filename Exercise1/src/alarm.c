#include "alarm.h"

//Initial alarm count is zero
unsigned int alarm_count = 0;
alarm_t alarms[MAX];


alarm_t get_alarm(unsigned int id)
{
    return alarms[id]; 
}

unsigned int get_alarm_count()
{
    return alarm_count;
}


//Helper Method: After entering the date and time for an alarm, create a new child process using fork(2)
unsigned int _fork(time_t timestamp)
{
    unsigned int pid = fork();

    if (pid != 0)
    { // Parent
        return pid;
    }
    else
    { // Child
        sleep(3);
        //For now, it is sufficient if “RING” is printed on the screen.
        printf("RING");
        exit(0);
        return 0;
    }
}

unsigned int schedule_alarm(time_t timestamp)
{
    //New alarm
    alarm_t new_alarm;
    new_alarm.timestamp = timestamp;

    //Child
    unsigned int pid = _fork(timestamp);
    new_alarm.pid = pid;

    //Designates an ID equal to the alarm count
    unsigned int alarm_id = alarm_count;
    alarms[alarm_id] = new_alarm;
    alarm_count += 1;


void cancel_alarm(unsigned int id)
{
    alarm_t alarm = get_alarm(id);
    kill(2);
    alarm_count -= 1;
}