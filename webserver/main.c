
#include "socket.h"
#include "signaux.h"



int main(void)
{
	
	/* Creer le serveur */
	int fd_serveur = creer_serveur(8080);

	initialiser_signaux();

	while(1) {
		int socket_client;
		/* Accepete la connection du client */
		socket_client = accept(fd_serveur, NULL, NULL);
		printf("Connection d'un client \n");
		if (socket_client == -1) {
			perror("Accept");
			return -1;
		}

		/* Creer le processus de client */
		int fils = fork();

		/* Gere le comportement du client */
		if (fils == 0)
		{

			const char *message_bienvenue = "Bonjour, bienvenue ! \n";

			/* Envoie le message de bienvenue */
			write(socket_client, message_bienvenue, strlen(message_bienvenue));

			char buf[512];

			int end = read(socket_client, buf, 512);

			/* Ecoute ce qu'il se passe et renvoie a tout les clients */
			while (end != 0) {
				printf("Message reçu : %s \n", buf);
				write(socket_client, buf, end);
				end = read(socket_client, buf, 512);
			}

			printf("Deconnection d'un client \n");
			exit(0);

		}

		/* Ferme la socket client sur le serveur */
		close(socket_client);

	}




	return 0;
}
