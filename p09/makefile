#Aprendido em https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
.PHONY: all clean

all: clean pingpong-sleep

pingpong-sleep: pingpong-sleep.c
	gcc -g -Wall -o pingpong-sleep pingpong-sleep.c pingpong.c queue.c timer.c

pingpong-join: pingpong-join.c
	gcc -g -Wall -o pingpong-join pingpong-join.c pingpong.c queue.c timer.c

clean:
	$(RM) pingpong-sleep pingpong-join
