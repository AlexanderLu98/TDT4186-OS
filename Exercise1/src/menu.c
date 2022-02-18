#include "alarm.h"

// Helper method to flush stdin.
void flush_in(void)
{
    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

int main() {
    // Variable that holds the users choice as a char
    char choice;

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
                    schedule_alarm(alarm_time);
                    break;
                }
            case 'l':
                list_active_alarms();
                break;
            case 'c':
                printf("C"); //cancel();
                break;
            case 'x':
                printf("Exiting");
                break;
            default:
                printf("Invalid input");
                break;
        }
        waitpid(-1, NULL, WNOHANG);
    }
    while(choice != 'x');
    return 0;
}
