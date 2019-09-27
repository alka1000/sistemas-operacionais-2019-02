#include <sys/time.h>
#include <signal.h>

struct sigaction action ;

// estrutura de inicialização to timer
struct itimerval timer;


void init_timer();