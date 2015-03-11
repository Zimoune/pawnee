#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>


enum http_method {
	HTTP_GET,
	HTTP_UNSUPPORTED,
};


typedef struct
{
	enum http_method method;
	int major_version;
	int minor_version;
	char * url;
} http_request;



