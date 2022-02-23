#include "alarm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

// Helper method to flush stdin.

void flush_in(void)
{
    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

int main(int argc, char *argv[]) {

    /*
    ================================================================================================
    Check for option flags. This program only allows for one option at the time.
    Available options are:
         -lt | --list-tracks            : list tracks in the ringtone folder
         -st | --set-track <track_num>  : set a track for the alarm by it's position listed by -lt
    ================================================================================================
    */

    // path to the ringtone tracks
    char *trackpath = "ringtones";
    // the default track choice is the first track in the folder
    int trackchoice = 0;

    if (argc > 1)
    {
        char *option = argv[1];

        if(strcmp("-lt", option) == 0 || strcmp("--list-tracks", option) == 0)
        {
            printf("Available tracks are: \n");
            list_available_alarm_tracks(trackpath);
            return 0;

        }
        else if(strcmp("-st", option) == 0 || strcmp("--set-track", option) == 0)
        {
            // check for empty string
            if(argc > 2 && strlen(argv[2]) != 0)
            {
                char * p;
                long tmp_arg = strtol(argv[2], &p, 10);

                // if there is no error, the long is casted as an int
                if(*p == '\0' && tmp_arg < INT_MAX && tmp_arg > INT_MIN)
                {
                    trackchoice = tmp_arg;
                }
                else
                {
                    printf("An error occured while parsing track number. Default 0 will be used.\n");
                }

                printf("Running alarm with track: %i  \n", trackchoice);
            }
        }
        else if (strcmp("-h", option) == 0 || strcmp("--help", option) == 0)
        {
            printf("================================================================================================\n");
            printf("Welcome to the alarm clock. The program can only parse one option at the time\n");
            printf("Available options are:\n");
            printf("     -lt | --list-tracks            : list tracks in the ringtone folder\n");
            printf("     -st | --set-track <track_num>  : set a track for the alarm by it's position listed by -lt\n");
            printf("     -h  | --help                   : displays the current menu\n");
            printf("================================================================================================\n");

            return 0;
        }
        else
        {
            printf("Unknown option. Please run the program with the -h or --help flag to view a list of valid options\n");
            printf("Terminating program\n");
            // return 1 signifying error from main.
            return 1;
        }
    }


    /*
    ================================================================================================
    END OF OPTIONS.

    START of loop for the menu and the alarm logic
    ================================================================================================
    */

    // Variable that holds the users choice as a char
    char choice;
    // status of the PID that just canceled
    int status;

    do {
        // Get the current time
        time_t mytime = time(NULL);
        char * time_str = ctime(&mytime);
        time_str[strlen(time_str)-1] = '\0';

        // Print Welcome screen with menu options
        printf("\nWelcome to the alarm clock! It is currently ");
        printf("%s",time_str);
        printf("\nPlease enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");

        // Read user input
        choice = getchar();
        flush_in();

        // variables for scheduling the alarm
        char alarm_time_str[] =  "YYYY-MM-DD HH:MM:SS";
        time_t alarm_time;
        struct tm alarm_time_tm;
        char *end;
        // sets daylight savings time, if not mktime will be one hour early.
        alarm_time_tm.tm_isdst = -1;

        char alarm_id = -1;

        // Determine what method should be called based on input
        switch (choice) {
            case 's':
                printf("Schedule alarm at which date and time? (YYYY-MM-DD HH:MM:SS): ");
                fgets(alarm_time_str, strlen(alarm_time_str) + 1, stdin);
                flush_in();

                // Coverts the user input to a tm struct, 'end' tracks success of failure
                end = strptime(alarm_time_str, "%Y-%m-%d %H:%M:%S", &alarm_time_tm);
                if (end == NULL || *end != '\0')
                {
                    printf("\nError in date and time input, alarm not scheduled.");
                    break;
                }
                else
                {
                    alarm_time = mktime(&alarm_time_tm);
                    printf("Attempting to schedule alarm at: %s", asctime(&alarm_time_tm));
                    schedule_alarm(alarm_time, trackpath, trackchoice);
                    break;
                }
            case 'l':
                list_active_alarms();
                break;
            case 'c':
                printf("Which alarm would you like to cancel?\n");
                // List all options
                list_active_alarms();
                printf("\nPlease enter the alarm ID: ");

                // Read input
                alarm_id = getchar();
                flush_in();

                // Cancel chosen alarm
                printf("\nAttempting to cancel alarm with ID: %c\n", alarm_id);

                //int id = (int) &alarm_id;
                cancel_alarm(alarm_id - '0');
                break;
            case 'x':
                printf("Exiting\n");
                break;
            default:
                printf("Invalid input\n");
                break;
        }
        waitpid(-1, &status, WNOHANG);
    }
    while(choice != 'x');
    return 0;
}
//2023-01-10 13:37:00