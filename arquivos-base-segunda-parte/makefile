#Aprendido em https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
.PHONY: all clean

all: clean pingpong-main-task

pingpong-main-task: pingpong-maintask.c
	gcc -g -Wall -o pingpong-maintask pingpong-maintask.c pingpong.c queue.c timer.c

clean:
	$(RM) pingpong-maintask
