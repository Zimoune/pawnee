
#include "socket.h"



int main(void)
{
	
	/* Creer le serveur */
	int fd_serveur = creer_serveur(8000);



	while(1) {
		int socket_client;
		/* Accepete la connection du client */
		socket_client = accept(fd_serveur, NULL, NULL);
		if (socket_client == -1) {
			perror("Accept");
			return -1;
		}
		const char *message_bienvenue = "Bonjour, bienvenue ! \n";

		/* Envoie le message de bienvenue */
		write(socket_client, message_bienvenue, strlen(message_bienvenue));

		char buf[512];

		int end = read(socket_client, buf, 512);

		/* Ecoute ce qu'il se passe et renvoie a tout les clients */
		while (end != 0) {
			write(socket_client, buf, end);
			end = read(socket_client, buf, 512);
		}
	}




	return 0;
}
