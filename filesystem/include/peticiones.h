#ifndef _PETICIONES_H_
#define _PETICIONES_H_
#include <sockets/sockets.h>
#include <pthread.h>
#include <manejo_de_filesystem.h>
#include <instrucciones_memoria/instrucciones_memoria.h>
#include <instrucciones_fs/instrucciones_fs.h>

extern int socket_kernel;
extern int socket_memoria;
extern t_log* log_filesystem;
extern pthread_t thread_peticion;
extern pthread_mutex_t lock_peticiones;


void* atender_peticion(t_buffer*);
bool existe_fcb(char*);
void enviar_creacion();
void enviar_truncamiento(char*, int);
void enviar_apertura(int);
void enviar_leer_escribir(char*, op_code);

#endif // !_PETICIONES_H_