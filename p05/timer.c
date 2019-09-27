#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

void init_timer(void(*handler)(int))
{
  action.sa_handler = handler ;
  sigemptyset (&action.sa_mask) ;
  action.sa_flags = 0 ;
  if (sigaction (SIGALRM, &action, 0) < 0)
  {
    perror ("Erro em sigaction: ") ;
    exit (1) ;
  }

  // ajusta valores do temporizador
  timer.it_interval.tv_usec = 1000 ;   // disparos subsequentes, em micro-segundos
  timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos
  timer.it_value.tv_usec = 1000 ;      // primeiro disparo, em micro-segundos
  //timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos

  // arma o temporizador ITIMER_REAL (vide man setitimer)
  if (setitimer (ITIMER_REAL, &timer, NULL) < 0)
  {
    perror ("Erro em setitimer: ") ;
    exit (1) ;
  }
}