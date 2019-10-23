#Aprendido em https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
.PHONY: all clean

all: clean pingpong-semaphore

pingpong-prodcons: pingpong-prodcons.c
	gcc -g -Wall -o pingpong-prodcons pingpong-prodcons.c pingpong.c queue.c timer.c

pingpong-racecond: pingpong-racecond.c
	gcc -g -Wall -o pingpong-racecond pingpong-racecond.c pingpong.c queue.c timer.c

pingpong-semaphore: pingpong-semaphore.c
	gcc -g -Wall -o pingpong-semaphore pingpong-semaphore.c pingpong.c queue.c timer.c

pingpong-sleep: pingpong-sleep.c
	gcc -g -Wall -o pingpong-sleep pingpong-sleep.c pingpong.c queue.c timer.c

pingpong-join: pingpong-join.c
	gcc -g -Wall -o pingpong-join pingpong-join.c pingpong.c queue.c timer.c

clean:
	$(RM) pingpong-sleep pingpong-join pingpong-racecond pingpong-semaphore pingpong-prodcons
