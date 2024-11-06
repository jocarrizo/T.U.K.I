#ifndef _CONEXIONES_KERNEL_H_
#define _CONEXIONES_KERNEL_H_
#include <sockets/sockets.h>

#include <conexion_memoria.h>
#include <conexion_cpu.h>
#include <conexion_filesystem.h>
#include <pthread.h>

extern int socket_kernel;

extern pthread_t conexion_memoria;
extern pthread_t conexion_cpu;
extern pthread_t conexion_fs;


void inicializar_threads_respuesta();
void esperar_finalizacion();
void liberar_conexiones();

#endif // !_CONEXIONES_KERNEL_H_