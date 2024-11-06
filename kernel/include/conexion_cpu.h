#ifndef _CONEXION_CPU_H_
#define _CONEXION_CPU_H_
#include <sockets/sockets.h>
#include <stdbool.h>

#include <planificacion.h>

extern int socket_cpu;
extern bool system_up;

void conectar_cpu(char* ip, char* puerto);
void* recibirMensajecpu(int* socket_cpu);

#endif // !_CONEXION_CPU_H_

