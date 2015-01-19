#ifndef __SOCKET_H__
#define __SOCKET_H__

/* Créé une socket serveur qui écoute sur toute les interfaces IPv4
de la machine sur le port passé en paramètre . La socket retournée
doit pouvoir être utilisée directement par un appel à accept .
La fonction retourne -1 en cas d ’ erreur ou le descripteur de la
socket créée . */

int creer_serveyr(int port);
#endif
