#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

int main() {

    char choice;

    do {
        //system("cls") //what
        
        printf("Welcome to the alarm clock! It is currently");
        printf("time");
        printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)");

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