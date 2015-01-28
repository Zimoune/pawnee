#include "socket.h"



int creer_serveur(int port){

	int socket_serveur;


	/* Creation de la socket serveur */
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_serveur == -1) {
		perror("socket_serveur");
		return -1;
	}		

	int optval = 1;

	/* Gere correctement l'arret du serveur */
	if (setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1 ) {
		perror("Can not set SO_REUSEADDR option");
		return -1;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;


	/* Attachement de la socket server sur toute les interfaces */
	if (bind(socket_serveur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		perror("bind socket_serveur");
		return -1;
	}

	initialiser_signaux();


	/* Demarre l'attente de connection */
	if (listen(socket_serveur, 10) == -1)
	{
		perror("listen socket_serveur");
		return -1;
	}

	return socket_serveur;


}


/* Gere les signaux */
void initialiser_signaux(void) 
{
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		perror("signal");
	}
}
