#Aprendido em https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
.PHONY: all clean

all: clean pingpong-join

pingpong-join: pingpong-join.c
	gcc -g -Wall -o pingpong-join pingpong-join.c pingpong.c queue.c timer.c

clean:
	$(RM) pingpong-join
