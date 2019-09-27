# compilador
CC = gcc

# flags
CFLAGS = -g -Wall

# executavel
TARGET = p02

# dependecias
DEPS = pingpong.h datatypes.h

# objetos
OBJ = pingpong.o pingpong-tasks3.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
