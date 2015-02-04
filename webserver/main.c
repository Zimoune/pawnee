
#include "socket.h"
#include "signaux.h"


void errorMessage(int socket_client) {
	char * message = "HTTP /1.1 400 Bad Request \r\nConnection : close \r\nContent-Length: 17 \r\n \r\n400 Bad Request \r\n";
	write(socket_client, message, strlen(message));
}

void answer(FILE * f) {
	printf("Answer ! \n");
	char * contenue = "Bienvenue sur notre site ! \r\n";
	int length = strlen(contenue);
	fprintf(f, "HTTP /1.1 200 OK \r\nConnection : close \r\nContent-Length: %d \r\n \r\n%s \r\n", length, contenue);
}

int parseRequest(char buf[], char get[], char slash[], int M, int m) {

	sscanf(buf, "%s %s HTTP/%d.%d", get, slash, &M, &m);

	if (strcmp(get, "GET") == 0 ) {
		if (M == 1 && (m == 1 || m == 0)) {
			printf("OK \n");
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}

}


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

			if (end == NULL) 
			{
				perror("BAD Request");
				return -1;
			} 


			// Recupere la requete du client et la test
			char get[20];
			char slash[20];
			int M = 0;
			int m = 0;

			if (parseRequest(buf, get, slash, M, m) == -1) {
				errorMessage(socket_client);
				return -1;
			}

			if ((end = fgets(buf, 512, f)) != NULL) {
				while (strcmp(buf, "\n") != 0 && strcmp(buf, "\r\n") != 0) {
					printf("buf : %s", buf);
					end = fgets(buf, 512, f);
				}
			}

			answer(f);



			/* Ecoute ce qu'il se passe et renvoie a tout les clients 
			while (end != NULL) {
				printf("%s", buf);
				end = fgets(buf, 512, f);
			} */


			exit(0);

		}

		/* Ferme la socket client sur le serveur */
		close(socket_client);

	}




	return 0;
}



