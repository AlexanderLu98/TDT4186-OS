#include "alarm.h"

void flush_in(void)
{
    int ch = 0;

    while ((ch = getchar()) != '\n' && ch != EOF);
}

int main() {

    // Variable that holds the users choice as a char
    char choice;

    // variables for switch
    char correct_format_str[] = "YYYY-MM-DD HH:MM:SS";
    char alarm_time_str[strlen(correct_format_str)];
    alarm_time_str[strlen(alarm_time_str)-1] = '\0';

    do {
        fflush(stdin);
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


        // Determine what method should be called based on input
        switch (choice) {
            case 's':
                printf("Schedule alarm at which date and time? (YYYY-MM-DD HH:MM:SS): ");
                fgets(alarm_time_str, strlen(correct_format_str) + 1, stdin);

                // creating a local scope within switch
                {
                    struct tm alarm_time_tm;
                    char *end = strptime(alarm_time_str, "%Y-%m-%d %H:%M:%S", &alarm_time_tm);
                    flush_in();
                    if (end == NULL || *end != '\0')
                    {
                        printf("\nError in data and time input, alarm not scheduled \n");
                        break;
                    }
                    else
                    {
                        printf("\nSucess! \n");
                        time_t alarm_time = mktime(&alarm_time_tm);
                        //printf("%s", asctime(&alarm_time));
                        schedule_alarm(alarm_time);
                        break;
                    }
                }
            case 'l':
                printf("L"); //list();
                break;
            case 'c':
                printf("C"); //cancel();
                break;
            case 'x':
                printf("X"); //exit();
                break;
            default:
                printf("Invalid input");
                break;
        }
    }
    while(choice != 'x');
    return 0;
}
