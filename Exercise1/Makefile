# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -pthread

# the build target executable:

TARGET = alarmclock

$(TARGET): src/menu.c src/alarm.c
	$(CC) $(CFLAGS) -o $(TARGET) src/*.c

clean:
	$(RM) $(TARGET)
