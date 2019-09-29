# compilador
CC = gcc

# flags
CFLAGS = -g -Wall

# executavel
TARGET = p04

# dependecias
DEPS = pingpong.h datatypes.h queue.h

# objetos
OBJ = queue.o pingpong.o pingpong-scheduler.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
