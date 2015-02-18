
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


// Envoie la premiere ligne de la reponse au client
void send_status(FILE * client , int code , const char * reason_phrase) {
	fprintf(client, "HTTP/1.1 %d %s \r\n", code, reason_phrase);	
}


// Renvoie la reponse au client
void send_response(FILE * client, int code, const char * reason_phrase, const char * message_body) {
	send_status(client, code, reason_phrase);
	int length = strlen(message_body);
	fprintf(client, "Connection: close \r\nContent-Length: %d \r\n\r\n%s", length, message_body);
}


int copy(int in, int out) {
	char buf[250];
	int r = read(in, buf, 250);
	while ( r != 0 ) 
	{
		write(out, buf, r);
		r = read(in, buf, 250);
	}

	return 1;
}


// Retourne la taille du fichier fd passer en parametre
int get_file_size(int fd) {
	struct stat file;

	if (fstat(fd, &file) == -1)
		return -1;

	return file.st_size;
}


// Renvoie au client le contenue avec l'entete 
void send_content(FILE * client, int socket_client, int code, const char * reason_phrase, int fd) {
	send_status(client, code, reason_phrase);
	int length = get_file_size(fd);
	fprintf(client, "Connection: close \r\nContent-Length: %d \r\n\r\n", length);
	fflush(client);
	copy(fd, socket_client);
}


// Enleve les elements inutiles de la requete
char * rewrite_url ( char * url ) {
	// Si rien n'est precise apres l'url on renvoie index.html
	if (strcmp(url, "/") == 0||strcmp(url, "") == 0 ) 
		return "/index.html";

	int length = strlen(url);
	char * res = malloc(length);

	int i = 0;
	while (url[i] != '?' && i < length) 
	{
		res[i] = url[i];
		i++;
	}

	return res;
}

// Test si le fichier demande existe et l'ouvre
int check_and_open ( const char * url , const char * document_root ) {
	
	// Verifie si l'utilisateur nessaye pas d'acceder a un fichier interdit
	if(strstr(url, "../") != NULL){
		return -2;
	}
	char * file_url = malloc(strlen(url)+strlen(document_root));
	strcpy(file_url, document_root);
	strcat(file_url, url);

	struct stat file;

	if ( stat(file_url, &file) == -1) {
		return -1;
	}
		
	// Test si le fichier est un fichier regulier
	if (!S_ISREG(file.st_mode)) {
		return -1;
	}

	int fd;

	// Essaye d'ouvrir le fichier
	if ((fd = open(file_url, O_RDONLY)) == -1) {
		return -1;
	}

	return fd;
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
	} 

	return 200;
}


int main (int argc, char *argv[])
{
	char * document_root = "content";

	if (argc == 2) {
		document_root = argv[1];
	}

	int fd;
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


			// recupere les eventuelles erreures de requete
			int erreur = getError(request);

			

			// passe les donnees inutiles de l'entete
			skip_headers(f);


			// Test les diverses erreurs
			if (erreur == 505) 
				send_response(f, 505 , "HTTP Version Not Supported" , "HTTP Version Not Supported \r\n");
			else if (erreur == 405)
				send_response(f , 405 , "Method Not Allowed" , "Method Not Allowed \r\n");
			else if (erreur == 200) {
				
				char * url = "";
				url = rewrite_url(request.url);				
				if ((fd = check_and_open(url, document_root)) == -1) {
					send_response(f, 404, "Not found", "Not Found \r\n");
				} else if (fd == -2) {
					send_response(f, 403, "Forbidden", "Forbidden \r\n");
				}else {
					// Si la requete est bonne on renvoie le contenu
					char * url = rewrite_url(request.url);
					int file = check_and_open(url, document_root);
					send_content(f, socket_client, 200, "OK", file);
				}

			} 


			exit(0);

		}

		/* Ferme la socket client sur le serveur */
		close(socket_client);

	}




	return 0;
}



