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