# compilador
CC = gcc

# flags
CFLAGS = -g -Wall

# executavel
TARGET = p03

# dependecias
DEPS = pingpong.h datatypes.h queue.h

# objetos
OBJ = queue.o pingpong.o pingpong-dispatcher.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
