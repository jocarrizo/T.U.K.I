#ifndef _PETICIONES_H_
#define _PETICIONES_H_
#include <instrucciones_memoria/instrucciones_memoria.h>
#include <commons/log.h>
#include <manejo_memoria_usuario.h>

extern t_log* log_memoria;
extern int socket_cpu;
extern int socket_fs;
extern int socket_kernel;

void realizar_peticion(t_buffer*, char*, int);
void inicializar_memoria();
void loguear_compactacion();

#endif // !_PETICIONES_H_