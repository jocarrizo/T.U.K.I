#ifndef _PETICIONES_MEMORIA_H_
#define _PETICIONES_MEMORIA_H_

#include <instrucciones_memoria/instrucciones_memoria.h>
#include <sockets/sockets.h>
#include <segmentos/segmentos.h>
#include <pcb.h>
#include <planificacion.h>

//Colas y Listas
extern t_list* tabla_procesos_activos;
//Socket
extern int socket_memoria;
//Var Globales
extern t_pcb* pcb_exec;
extern t_buffer* buff_memoria;
//Semaforos
extern sem_t recepcion_tabla, recepcion_base, buff_memoria_sem, tabla_actualizada_sem, tablas_actualizadas_sem;



t_tabla_de_segmentos* asignar_memoria(int pid);
void pedir_segmento(int tamanio,int pid, int id_seg);
int recibir_base();
void asignar_segmento(int base);
void pedir_compactacion();
void eliminar_segmento(int pid, int id_seg);
void asignar_tabla_actualizada();
void liberar_memoria(int pid);
void actualizar_tabla(t_tabla_de_segmentos*);
#endif /* _PETICIONES_MEMORIA_H_ */