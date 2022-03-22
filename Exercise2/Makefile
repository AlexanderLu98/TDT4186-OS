# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:

TARGET = mtwwwd

$(TARGET): src/mtwwwd.c src/sem.c src/bbuffer.c
	$(CC) $(CFLAGS) -o $(TARGET) src/mtwwwd.c src/sem.c src/bbuffer.c

clean:
	$(RM) $(TARGET)
