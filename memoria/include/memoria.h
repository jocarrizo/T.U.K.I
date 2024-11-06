#ifndef _MEMORIA_H_
#define _MEMORIA_H_
#include <conexiones_memoria.h>
#include <segmentos/segmentos.h>
#include <pthread.h>
// CORRESPONDIENTE A SEMAFORO


//IMPLEMENTAR MUTEX PARA LAS VARIABLES COMPARTIDAS (MEMORIA_USUARIO, TABLAS_DE_SEGMENTOS, HUECOS_LIBRES, SEGMENTO_0)

// SEMAFOROS
sem_t memoria_usuario_sem;
sem_t tablas_de_segmentos_sem;
sem_t huecos_libres_sem;





int id_memoria = 6;
t_config* config;
t_log* log_memoria;

void* memoria_usuario;
t_list* tablas_de_segmentos;
t_list* huecos_libres; // LISTA DE HUECOS LIBRES
t_segmento* segmento_0;
#endif // !_MEMORIA_H_