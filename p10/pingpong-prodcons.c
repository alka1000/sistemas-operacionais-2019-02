#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"


// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

#define NUMPRODUTORES 3
#define NUMCONSUMIDORES 2
#define TAMANHOBUFFER 5

task_t produtores[NUMPRODUTORES];
task_t consumidores[NUMCONSUMIDORES];
semaphore_t  s_buffer, s_item, s_vaga;
int item = 0 ;
long int buffer[5];
int tamBuf = 0;

void consumidor(void *id)
{
	while(true) {
		sem_down(&s_item);

		sem_down(&s_buffer);
		
		item = buffer[0];
		for (int i = 0; i<tamBuf-1; i++) {
			buffer[i] = buffer[i+1];
		}
		tamBuf--;

		sem_up(&s_buffer);
		sem_up(&s_vaga);

		printf("%s consumiu %d\n", (char*)id, item);
		task_sleep(1);
	}
	// task_exit (0) ;
}

void produtor(void *id)
{
	while(true) {
		task_sleep(1);
		item = random() % 99 ;
		sem_down(&s_vaga);
		sem_down(&s_buffer);
		buffer[tamBuf] = item;
		tamBuf++;
		printf("%s produziu %d\n", (char*)id, item);
		
		sem_up(&s_buffer);

		sem_up(&s_item);
	}
	// task_exit (0) ;
}

int main (int argc, char *argv[])
{

	printf ("Main INICIO\n") ;

	pingpong_init () ;

	sem_create (&s_vaga, 5) ;
	sem_create (&s_item, 0) ;
	sem_create (&s_buffer, 1) ;


	task_create (&consumidores[0], consumidor, "                     c1") ;
	task_create (&consumidores[1], consumidor, "                     c2") ;
	task_create (&produtores[0], produtor, "p1") ;
	task_create (&produtores[1], produtor, "p2") ;
	task_create (&produtores[2], produtor, "p3") ;


	task_exit (0) ;

}
