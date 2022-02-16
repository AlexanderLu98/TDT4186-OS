#include <stdio.h>
#include <time.h>
#include <string.h>

int main() {

    // Variable that holds the users choice as a char
    char choice;

    // Get the current time
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

    do {
        // Print Welcome screen with menu options
        printf("\nWelcome to the alarm clock! It is currently ");
        printf("%s",time_str);
        printf("\nPlease enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");

        // Read user input
        scanf("%c", &choice);

        // Determine what method should be called based on input
        switch (choice) {
            case 's':
                printf("S"); //schedule();
                break;
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
    while(choice!=99); 
    return 0;
}