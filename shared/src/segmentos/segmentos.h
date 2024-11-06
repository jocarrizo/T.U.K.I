#ifndef _SEGMENTOS_H_
#define _SEGMENTOS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sockets/sockets.h>
#include <commons/collections/list.h>

typedef struct{
	int base;
	int tamanio;
}t_segmento;

typedef struct{
	int pid;
	int tam;
	t_segmento** segmentos; //TODO: REVER ESTO, no es implementable pq terminamos usando las commons lists
}t_tabla_de_segmentos;


void liberar_segmento(t_segmento*);
t_segmento* desempaquetar_segmento(void*, int*);
void empaquetar_tabla_segs(t_paquete*, t_tabla_de_segmentos*);
t_tabla_de_segmentos* desempaquetar_tabla_segs(void*, int*);
void liberar_tabla_segs(t_tabla_de_segmentos*);
void liberar_tabla_segs_completa(t_tabla_de_segmentos*);
void enviar_tabla_segs(t_tabla_de_segmentos*, int, int);
t_tabla_de_segmentos* crear_tabla_segs(int, int);



#endif // !_SEGMENTOS_H_