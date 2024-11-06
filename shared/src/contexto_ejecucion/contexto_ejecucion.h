#ifndef _CONTEXTO_EJECUCION_H_
#define _CONTEXTO_EJECUCION_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <assert.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include <segmentos/segmentos.h>
#include <registros/registros.h>
#include <instrucciones/instrucciones.h>
#include <string.h>




typedef struct{
	uint id;//2
	//uint32_t tamanio;//4
	t_registros registros;//112	
	t_list *instrucciones;// 4 + ?
	uint32_t program_counter;// 4
	double tiempo_real_ej;
	t_tabla_de_segmentos *tabla_segmentos;
}t_contexto_ejecucion;

void imprimir_contexto(t_contexto_ejecucion* contexto);
void imprimir_registros(t_registros *regs);
void enviar_contexto(t_contexto_ejecucion* contexto, int socket_a_enviar, int Codigo);


t_contexto_ejecucion* desempaquetar_contexto(void* buffer);
void liberar_contexto(t_contexto_ejecucion* contexto);

void empaquetar_contexto(t_paquete*, t_contexto_ejecucion*);

//void inicializar_registros(t_pcb*);

#endif /* _CONTEXTO_EJECUCION_H_ */
