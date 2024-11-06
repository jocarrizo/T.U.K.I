#ifndef _CONEXION_KERNEL_H_
#define _CONEXION_KERNEL_H_
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <commons/log.h>
#include <sockets/sockets.h>
#include <leer_config.h>

extern t_log *log_consola;

int conectar_a_kernel();
bool conexion_exitosa(int);
void enviar_paquete_instrucciones(t_paquete *paquete, int socket_kernel);
bool terminar(int);
#endif // !_CONEXION_KERNEL_H_