#Aprendido em https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

pingpong-contab-prio: pingpong-contab-prio.c
	gcc -g -Wall -o pingpong-contab-prio pingpong-contab-prio.c pingpong.c queue.c timer.c
pingpong-contab: pingpong-contab.c
	gcc -g -Wall -o pingpong-contab pingpong-contab.c pingpong.c queue.c timer.c
clean:
	$(RM) p02
