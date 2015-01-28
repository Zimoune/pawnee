#include "socket.h"



int creer_serveur(int port){

	int socket_serveur;


	/* Creation de la socket serveur */
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_serveur == -1) {
		perror("socket_serveur");
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


	/* Demarre l'attente de connection */
	if (listen(socket_serveur, 10) == -1)
	{
		perror("listen socket_serveur");
		return -1;
	}

	return socket_serveur;


}
