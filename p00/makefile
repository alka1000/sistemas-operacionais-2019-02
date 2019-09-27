# compilador
CC = gcc

# flags
CFLAGS = -g -Wall

# executavel
TARGET = p00

# dependecias
DEPS = queue.h

# objetos
OBJ = queue.o testafila.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
