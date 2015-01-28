
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
		if (socket_client == -1) {
			perror("Accept");
			return -1;
		}

		/* Creer le processus de client */
		int fils = fork();

		/* Gere le comportement du client */
		if (fils == 0)
		{

			/* const char *message_bienvenue = "Bonjour, bienvenue ! \n";

			// Affichage du message de bienvenue
			write(socket_client, message_bienvenue, strlen(message_bienvenue)); */

			char buf[512];

			const char * mode = "w+";
			FILE *f = fdopen(socket_client, mode);
			if (f == NULL) 
			{
				perror("fdopen");
				return -1;
			}



			char * end = fgets(buf, 512, f);

			/* Ecoute ce qu'il se passe et renvoie a tout les clients */
			while (end != NULL) {
				fprintf(f, "<pawnee> %s", buf);
				end = fgets(buf, 512, f);
			}


			exit(0);

		}

		/* Ferme la socket client sur le serveur */
		close(socket_client);

	}




	return 0;
}
