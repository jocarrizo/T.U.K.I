#ifndef _CONEXION_FILESYSTEM_H_
#define _CONEXION_FILESYSTEM_H_
#include <sockets/sockets.h>
#include <stdbool.h>
#include <semaphore.h>
#include <commons/log.h>

extern int socket_fs;
extern t_buffer* buff_fs; 
extern sem_t apertura_sem;
extern sem_t creacion_sem;
extern sem_t truncamiento_sem;
extern sem_t buff_fs_sem;
extern sem_t leer_sem;
extern sem_t escribir_sem;
extern t_log* log_kernel;
extern bool system_up;
extern int socket_kernel;
extern int socket_memoria;
extern int socket_cpu;

void conectar_fs(char* ip, char* puerto);
void* recibirMensajefs(int* socket_fs);

#endif // !_CONEXION_FILESYSTEM_H_
