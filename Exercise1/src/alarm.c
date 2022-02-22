#include "alarm.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>

//Initial alarm count is zero
unsigned int alarm_count = 0;
alarm_t alarms[MAX];
int status;

alarm_t get_alarm(unsigned int id)
{
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
        if(alarms[i].active == 0)
        {
            return i;
        }
    }
    // if no free spot is found return -1
    return -1;
}

//Helper Method: After entering the date and time for an alarm, create a new child process using fork(2)
unsigned int _fork(time_t timestamp, char * trackpath, int tracknum)
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
        play_alarm_sound(trackpath, tracknum);

        // clean up, setting the alarm to inactive, and exiting the process
        exit(pid);
        return 0;
    }
}

unsigned int schedule_alarm(time_t timestamp, char * trackpath, int tracknum)
{
    int alarm_id = get_free_alarm_id();
    if(alarm_id != -1)
    {
        //New alarm
        alarm_t new_alarm;
        new_alarm.timestamp = timestamp;
        new_alarm.active = 1;

        //Child
        unsigned int pid = _fork(timestamp, trackpath, tracknum);
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
    if(0 <= id && id < MAX)
    {
        alarm_t alarm = get_alarm(id);
        if(alarm.pid > 0)
        {
            kill(alarm.pid, 0);
            // Set all the alarm values as 0, making room for new alarms
            alarms[id].active    = 0;
            alarms[id].pid       = 0;
            alarms[id].timestamp = 0;
        }
        else
        {
            printf("The alarm at the given index %ui does not have a valid PID, no alarms canceled.\n", id);
        }
    }
    else
    {
        printf("Index %ui is out of bounds, no alarms canceled.\n", id);

    }
}

void list_active_alarms()
{
    printf("LIST OF ACTIVE ALARMS: \n");
    for(int i = 0; i<MAX; i++)
    {
        if(alarms[i].active == 1)
        {
            printf("ID: %i PID: %u ACTIVE: %i TIME: %s", i, alarms[i].pid, alarms[i].active, ctime(&alarms[i].timestamp));
        }
    }
    printf("END OF LIST. \n");
}


void list_available_alarm_tracks(char * path)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */
        int counter = 0;
        printf("TRACK NUM   TRACK NAME \n");
        while ((ent = readdir (dir)) != NULL) {
            if(strcmp("..", ent->d_name) != 0 && strcmp(".", ent->d_name) != 0 )
            {
                printf ("    %i       %s \n", counter, ent->d_name );
                counter ++;
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
    }
}


int get_available_alarm_tracks(char * path, char ** tracklist)
{
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */
        int counter = 0;
        while ((ent = readdir (dir)) != NULL && counter < MAX_TRACKS) {
            if(strcmp("..", ent->d_name) != 0 && strcmp(".", ent->d_name) != 0 )
            {
                // This is probably horrible and leaky...
                tracklist[counter] = malloc((strlen(path) + strlen(ent->d_name) + strlen("/") + 1)*sizeof(char));
                strcpy(tracklist[counter], path);
                strcat(tracklist[counter],"/");
                strcat(tracklist[counter], ent->d_name);
                counter ++;
            }
        }
        closedir (dir);
        return counter;
    } else {
        /* could not open directory */
        perror ("");
        return -1;
    }
}

void play_alarm_sound(char *path, int index)
{
    int trackcount;
    char ** tracklist = calloc(MAX_TRACKS, sizeof(char*));
    // this should rather be stored as a list in the main function, than being run over again.
    trackcount = get_available_alarm_tracks(path, tracklist);
    if(index >= 0 && index < trackcount)
    {
        if(SYSTYPE == 2)
        {
            char command[1000] =  "mpg123 --quiet ";
            strcat(command, tracklist[index]);
            system(command);
        }
        else if (SYSTYPE == 1)
        {
            char command[1000] ="afplay --quiet ";
            strcat(command, tracklist[index]);
            system(command);
        }
        else
        {
            printf("RING RING! This program does not support audio alarms on your system. \n");
        }
    }
    else
    {
        printf("RING RING! Provided index is out of range.\n");
    }
    // freeing the allocated memory
    for(int i=0; i<MAX_TRACKS;i++) free(tracklist[i]);
    free(tracklist);
}
