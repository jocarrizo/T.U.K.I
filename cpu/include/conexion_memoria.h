#ifndef _CONEXIOn_MEMORIA_H_
#define _CONEXIOn_MEMORIA_H_
#include <sockets/sockets.h>
#include <stdbool.h>
#include <commons/log.h>

extern int socket_memoria;
extern int id_cpu;
extern t_log* log_cpu;

void conectar_memoria(char* ip, char* puerto);
void* recibirMensajeMemoria(int* socket_memoria);

#endif // !_CONEXIOn_MEMORIA_H_
