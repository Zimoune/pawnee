
#include "socket.h"
#include "signaux.h"
#include "pawnee.h"


// Essaye de faire un fgets, quitte le processus sinon
char * fgets_or_exit (char * buffer , int size , FILE * stream) {
	char * c;
	if ((c = fgets(buffer, size, stream)) == NULL)
		exit(1);

	return c;
} 

void send_status(FILE * client , int code , const char * reason_phrase) {
	fprintf(client, "HTTP/1.1 %d %s \r\n", code, reason_phrase);	
}


// Renvoie la reponse au client
void send_response(FILE * client, int code, const char * reason_phrase, const char * message_body) {
	send_status(client, code, reason_phrase);
	int length = strlen(message_body);
	fprintf(client, "Connection: close \r\nContent-Length: %d \r\n\r\n%s", length, message_body);
}


// Parse la requete du client dans la structure http_request
int parse_http_request (const char * request_line , http_request * request) {
	char methode[10];
	char * url;
	url = malloc(256);
	if (url == NULL) {
		return -1;
	}
	
	printf("Recu : %s \n", request_line);

	sscanf(request_line, "%s %s HTTP/%d.%d", methode, url, &(*request).major_version, &(*request).minor_version);


	request->url = url;
	if (strcmp(methode, "GET") == 0) {
		(*request).method = HTTP_GET;
	} else {
		(*request).method = HTTP_UNSUPPORTED;
	}
	return 1;
}

// Passe les ligne inutiles de l'entete
void skip_headers( FILE * f) {
	char buf[512];

	fgets_or_exit(buf, 512, f);
	while (strcmp(buf, "\n") != 0 && strcmp(buf, "\r\n") != 0) {
		fgets_or_exit(buf, 512, f);
	}
}


// Renvoie si il y a des erreurs ou 200 sinon
int getError(http_request request) {
	if (request.major_version != 1||(request.minor_version != 0 && request.minor_version != 1)) {
		return 505;
	} else if (request.method == HTTP_UNSUPPORTED) {
		return 405;
	} else if (strcmp(request.url , "/" ) == 0) {
		return 200;
	}

	return 0;
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



			char buf[512];

			const char * mode = "w+";
			FILE *f = fdopen(socket_client, mode);
			if (f == NULL) 
			{
				perror("fdopen");
				return -1;
			}



			// Recupere la requete du client et la test
			fgets_or_exit(buf, 512, f);
			http_request request;
			parse_http_request(buf, &request);

			int erreur = getError(request);

			// passe les donnees inutiles de l'entete
			skip_headers(f);


			// Test les diverses erreyrs
			if ( erreur == 400 )
				send_response(f, 400 , "Bad Request" , "Bad request \r\n");
			else if (erreur == 505) 
				send_response(f, 505 , "HTTP Version Not Supported" , "HTTP Version Not Supported \r\n");
			else if (erreur == 405)
				send_response(f , 405 , "Method Not Allowed" , "Method Not Allowed \r\n");
			else if (erreur == 200)
				send_response(f, 200,"OK", "Hello World ! \r\n");
			else
				send_response(f, 404, "Not Found", "Not Found \r\n");


			exit(0);

		}

		/* Ferme la socket client sur le serveur */
		close(socket_client);

	}




	return 0;
}



