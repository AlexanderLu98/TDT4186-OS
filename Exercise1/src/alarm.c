#include "alarm.h"

//Initial alarm count is zero
unsigned int alarm_count = 0;
alarm_t alarms[MAX];

alarm_t get_alarm(unsigned int id)
{
    printf("id: %u", id);
    printf("pid: %u", alarms[id].pid);
    return alarms[id];
}

unsigned int get_alarm_count()
{
    return alarm_count;
}


// iterates over the alarm list and finds and open space if it is available.
int get_free_alarm_id()
{
    for(int i = 0; i<MAX; i++)
    {
        printf("%i:  %i  \n", i,  alarms[i].active);
        if(alarms[i].active == 0)
        {
            return i;
        }
    }
    // if no free spot is found return -1
    return -1;
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
    {   // Child
        sleep(timestamp - time(NULL));

        //Playing alarm sound
        play_alarm_sound();

        // clean up, setting the alarm to inactive, and exiting the process
        exit(pid);
        return 0;
    }
}

unsigned int schedule_alarm(time_t timestamp)
{
    int alarm_id = get_free_alarm_id();
    if(alarm_id != -1)
    {
        //New alarm
        alarm_t new_alarm;
        new_alarm.timestamp = timestamp;
        new_alarm.active = 1;

        //Child
        unsigned int pid = _fork(timestamp);
        new_alarm.pid = pid;

        //Designates an ID equal to the alarm count
        alarms[alarm_id] = new_alarm;
        alarm_count += 1;
    }
    else
    {
        printf("Maximum number of alarms reached. Choose \"l\" to list active alarms, and try again later.\n");
    }

    return 0;
}

void cancel_alarm(unsigned int id)
{
    // The alarm needs to be removed from the list after being canceled, at this time nothing happens to it.
    alarm_t alarm = get_alarm(id);
    kill(alarm.pid, SIGKILL);
    // Set the alarm as inactives

    alarms[id].active = 0;
    alarm_count -= 1;
    printf("test id: %c", id);
}

void list_active_alarms()
{
    printf("LIST OF ACTIVE ALARMS: \n");
    for(int i = 0; i<MAX; i++)
    {
        if(alarms[i].active == 1)
        {
            printf("ID: %i - PID: %u - ACTIVE - TIME: %s", i, alarms[i].pid, ctime(&alarms[i].timestamp));
        }
        else
        {
            printf("ID: %i - PID: %u - NOT ACTIVE - TIME: %s", i, alarms[i].pid, ctime(&alarms[i].timestamp));
        }
    }
    printf("END OF LIST. \n");
}


void play_alarm_sound()
{
    if(SYSTYPE == 2) system("mpg123 --quiet ringtones/sea_shanty.mp3");
    else if (SYSTYPE == 1) system("afplay ringtones/sea_shanty.mp3");
}
//2023-01-10 13:37:00