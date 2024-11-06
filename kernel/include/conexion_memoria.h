#ifndef _CONEXION_MEMORIA_H_
#define _CONEXION_MEMORIA_H_
#include <stdbool.h>
#include <sockets/sockets.h>
#include <commons/log.h>
#include <semaphore.h>

extern int socket_memoria;
extern int id_kernel;
extern t_log* log_kernel;
extern sem_t recepcion_tabla,recepcion_base, buff_memoria_sem, tabla_actualizada_sem, eliminacion_proceso_sem, tablas_actualizadas_sem;
extern t_buffer* buff_memoria;
extern bool system_up;
extern int socket_kernel;
extern int socket_cpu;
extern int socket_fs;

void conectar_memoria(char* ip, char* puerto);
void* recibirMensajeMemoria(int* socket_memoria);
#endif // !_CONEXION_MEMORIA_H_
