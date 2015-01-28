#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


void traitement_signal(int sig);
void initialiser_signaux(void);