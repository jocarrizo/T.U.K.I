#ifndef _PLANIFICACION_H_
#define _PLANIFICACION_H_

#include <pthread.h>
#include <semaphore.h>

#include <pcb.h>
#include <sockets/sockets.h>

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <string.h>

#include <leer_config.h>
#include <conexiones_kernel.h>
#include <recursos.h>
#include <peticiones_memoria.h>

#include <peticiones_fs.h>

typedef enum{
    FIN_TRUNCAMIENTO, FIN_OP_FS_MEMORIA
}id_evento; 

typedef struct{
    t_pcb* pcb;
    int tiempo;
}t_block;

//hilos
extern pthread_t hilo_ready;
extern pthread_t hilo_block;
extern pthread_t hilo_exec;
extern pthread_t hilo_exit;
extern pthread_t hilo_bloqueo_io;
extern pthread_t hilo_bloqueo_fs;



void *thread_ready();
void *thread_exec();
void *thread_block();
void *thread_exit();


//Colas y Listas
extern t_queue *new_queue;
extern t_list *ready_list,*block_list,*exit_list;
extern t_queue* *bloqueado_recurso;

//Semaforos
extern sem_t new_sem;
extern sem_t ready_sem;
extern sem_t block_sem;
extern sem_t exit_sem;

extern sem_t grado_multiprogramacion_sem;
extern sem_t recepcion_tabla;

extern sem_t sock_libre;

extern sem_t hay_op_FS_MEM, contador_op_FS_MEM; //Corrobora que no haya operaciones actuales entre FS y memoria.
extern sem_t tabla_procesos_activos_sem;
//Contadores
extern sem_t new_contador_sem, ready_contador_sem, exit_contador_sem;

//Var globales
extern t_pcb* pcb_exec;
extern int new_contador, proceda;
extern int dir_fisica;

//Cronometro
extern t_temporal* tiempo;

//Sockets
extern int socket_cpu;
extern int socket_memoria;

//Log
extern t_log* log_kernel;

void inicializarColas();

void inicializarColasBloqueo();

void inicializarSemaforos();

void asignarTareasAHilos();

/*****************************NEW**************************************/

void agregar_new(t_pcb* pcb);

t_pcb* sacar_new(/*TODO : cambiar retorno a pcb*/);

/* ****************************READY************************************* */

void new_a_ready(t_pcb* pcb);

void agregar_ready(t_pcb* pcb);

t_pcb *sacar_ready();

t_pcb* maximo(t_pcb* A,t_pcb* B);

double hrrn_pcb(t_pcb*);

/*****************************BLOCK**************************************/

void agregar_block(t_pcb* pcb , int id);

void sacar_block(int id);

void *block_io(t_block* s_bloqueo);

void bloquear_io(t_pcb* pcb, int tiempo);
/* ****************************EXIT************************************* */
void agregar_exit(t_pcb* pcb, char motivo[]);

t_pcb* sacar_exit();

void *thread_ready();

void *thread_exec();

void *thread_exit();

void liberar_tabla_archivos(t_list**);

void sacar_de_procesos_block(t_archivo*);
void realizar_op(op_code cod_op);
void reestimar_rafaga_proceso();
t_instruccion* instruccion_actual();

int tamanio_segmento_pedido();
int id_segmento_pedido();
int id_pcb_actual();
int buscar_puntero(char*);

void cerrar_archivo(char* );
void* esperar_truncamiento();
void* esperar_ok_fs(void*);
#endif // !_PLANIFICACION_H_