# compilador
CC = gcc

# flags
CFLAGS = -g -Wall

# executavel
TARGET = p09

# dependecias
DEPS = pingpong.h datatypes.h queue.h timer.h

# objetos
OBJ = queue.o timer.o pingpong.o pingpong-sleep.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
