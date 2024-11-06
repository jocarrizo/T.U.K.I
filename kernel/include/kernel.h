#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include <commons/config.h>
#include <commons/log.h>

#include <conexion_consola.h>
#include <recursos.h>

int socket_cpu = -1;
int socket_fs = -1;
int socket_memoria = -1;
int socket_kernel = -1;

pthread_t conexion_memoria;
pthread_t conexion_cpu;
pthread_t conexion_fs;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



int cant_procesos = 1;
int id_kernel = 0;
t_log* log_kernel;
t_config* config;


//PLANIFICACION

//Hilos
pthread_t hilo_ready;
pthread_t hilo_block;
pthread_t hilo_exec;
pthread_t hilo_exit;
pthread_t hilo_bloqueo_io;
pthread_t hilo_bloqueo_fs;


//Listas
t_queue *new_queue;
t_list *ready_list,*block_list,*exit_list;
t_queue* *bloqueado_recurso;
t_list *tabla_procesos_activos;
//Semaforos para meter y sacar de colas
sem_t new_sem;
sem_t ready_sem;
sem_t block_sem;
sem_t exit_sem;

sem_t grado_multiprogramacion_sem;

sem_t cpu_ready;
sem_t sock_libre;
sem_t hay_op_FS_MEM, contador_op_FS_MEM;
sem_t tabla_procesos_activos_sem;

sem_t recepcion_tabla, recepcion_base, buff_memoria_sem, tabla_actualizada_sem, eliminacion_proceso_sem, tablas_actualizadas_sem;
//Contadores
sem_t new_contador_sem,ready_contador_sem,exit_contador_sem;

//Var globales
t_pcb* pcb_exec;
int new_contador, proceda = 1;
t_buffer* buff_memoria;
t_list* tabla_global_archivos_abiertos;
t_buffer* buff_fs;
int dir_fisica;
bool system_up = true;
//Cronometro
t_temporal* tiempo;

//PLANIFICACION

//RECURSOS

t_list* recursos;

//RECURSOS
//semaforo msj fs
sem_t apertura_sem;
sem_t creacion_sem;
sem_t truncamiento_sem;
sem_t buff_fs_sem;
sem_t leer_sem;
sem_t escribir_sem;

#endif // !_KERNEL_H_
