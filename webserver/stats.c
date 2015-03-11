#include "stats.h"
#include <stdio.h>
#include <sys/mman.h>
#include <semaphore.h>



static web_stats * stats;
static sem_t * sem;


int init_stats (void)
{


	stats = mmap(NULL,sizeof(web_stats),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	stats->served_connections = 0;
	stats->served_requests = 0;
	stats->ok_200 = 0;
	stats->ko_400 = 0;
	stats->ko_403 = 0;
	stats->ko_404 = 0;

	sem = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

	return 1;
}

web_stats * get_stats (void)
{
	return stats;
}

sem_t * get_sem(void) 
{
	return sem;
}



