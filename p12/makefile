# compilador
CC = gcc

# flags
CFLAGS = -g -Wall

# executavel
TARGET = p12

# dependecias
DEPS = pingpong.h datatypes.h queue.h timer.h

# objetos
OBJ = queue.o timer.o pingpong.o pingpong-mqueue.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
