#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_
#include <pthread.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <conexiones_filesystem.h>


int socket_fs = -1;
int socket_kernel = -1;
int socket_memoria = -1;
int id_fs = 1;

pthread_t thread_peticion;
pthread_mutex_t lock_peticiones = PTHREAD_MUTEX_INITIALIZER;

t_log* log_filesystem;
t_config* config;
t_config* config_superbloque;
t_list* fcbs;

char* mapeo_bitmap_en_memoria;
t_bitarray* bitmap;
char* blocks;
#endif // !_FILESYSTEM_H_

