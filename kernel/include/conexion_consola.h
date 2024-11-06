#ifndef _CONEXION_CONSOLA_H_
#define _CONEXION_CONSOLA_H_
#include <pthread.h>
#include <stdbool.h>
#include <sockets/sockets.h>
#include <planificacion.h>
#include <commons/log.h>
#include <leer_config.h>
#include <pcb.h>

extern int cant_procesos;
extern t_log* log_kernel;
extern pthread_mutex_t lock;
extern bool system_up;

void conectar_consola(int servidor);
void* proceso_new(int* socket_consola);
t_pcb* generar_pcb(int socket_cliente);
void recibir_paquete_consola(void *buffer, int size, t_pcb *pcb);
#endif // !_CONEXION_CONSOLA_H_