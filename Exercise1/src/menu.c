#include <stdio.h>
#include <time.h>
#include <string.h>

int main() {

    char choice;

    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

    do {
        //system("cls") //what
        //printf("\n");
        printf("\nWelcome to the alarm clock! It is currently ");
        printf("%s",time_str);
        printf("\nPlease enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");

        scanf("%c", &choice);

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