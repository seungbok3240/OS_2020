.SUFFIXES : .c .o

OBJECTS = main.o read.o write.o

CC = gcc
CFLAGS = -g

TARGET = test

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) -c $^

clean :
	rm -rf $(OBJECTS) $(TARGET) test.txt

