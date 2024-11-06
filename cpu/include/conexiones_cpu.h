#ifndef _CONEXIONES_CPU_H_
#define _CONEXIONES_CPU_H_
#include <conexion_kernel.h>
#include <commons/log.h>
#include <conexion_memoria.h>
#include <pthread.h>

extern int socket_cpu;
extern int socket_memoria;
extern pthread_t conexion_memoria;

void liberar_conexiones();
void inicializar_conexiones();
#endif // !_CONEXIONES_CPU_H_