#ifndef _CONEXION_MEMORIA_H_
#define _CONEXION_MEMORIA_H_
#include <sockets/sockets.h>
#include <commons/log.h>
#include <stdbool.h>

extern int id_fs;
extern int socket_memoria;
extern t_log* log_filesystem;

void conectar_memoria(char* ip, char* puerto);

#endif // !_CONEXION_MEMORIA_H_