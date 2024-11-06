#ifndef _CONEXIONES_MEMORIA_H_
#define _CONEXIONES_MEMORIA_H_
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <peticiones.h>

extern int id_memoria;
extern t_log* log_memoria;

int inicializarConexiones();
void* conectar_con_cpu(int* socket_cpu);
void* conectar_con_fs(int* socket_fs);
void* conectar_con_kernel(int* socket_kernel);

#endif // !_CONEXIONES_MEMORIA_H_