#include "signaux.h"

void traitement_signal(int sig)
{
	/* Attend le processus fils pour le quitter proprement */
	printf ("Signal %d reçu \n" , sig);
	waitpid(-1, NULL, WNOHANG);
}

/* Gere les signaux */
void initialiser_signaux(void) 
{
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		perror("signal");
	}

	struct sigaction sa;

	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction(SIGCHLD)");
	}
}
