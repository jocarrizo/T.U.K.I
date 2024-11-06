#ifndef _CPU_H_
#define _CPU_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <conexiones_cpu.h>
#include <leer_config.h>
#include <commons/temporal.h>

int socket_cpu = -1;
int socket_kernel = -1;
int socket_memoria = -1;
int id_cpu = 2;
int direccion_fisica = 0;
int desplazamiento_segmento = 0;
int num_segmento;

t_registros regs;
t_log* log_cpu;
t_config* config;
pthread_t conexion_memoria;
t_temporal* tiempo_real_ejecucion;
t_segmento* seg_trabajo;

#endif // !_CPU_H_