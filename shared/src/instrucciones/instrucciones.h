#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <stdint.h>
#include <assert.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sockets/sockets.h>
#include <commons/log.h>
#include <math.h>
typedef enum {
F_WRITE,F_READ,SET,MOV_IN,MOV_OUT, F_TRUNCATE,F_SEEK,CREATE_SEGMENT,IO,WAIT,SIGNAL,F_OPEN,F_CLOSE,DELETE_SEGMENT,EXIT,YIELD,NO_RECONOCIDO
} id;

typedef struct {
	uint8_t identificador;
	uint8_t cant_parametros;
	char ** parametros;
} t_instruccion;

uint8_t get_identificador(char *);
char* get_nombre_instruccion(uint8_t);
uint8_t get_cant_parametros(uint8_t);
t_instruccion *crear_instruccion(uint8_t, uint8_t, char**);
void empaquetar_instruccion(t_paquete *paquete, t_instruccion *i);
void imprimir_instruccion(t_instruccion *);
void liberar_instruccion(t_instruccion *);
t_instruccion *desempaquetar_instruccion(void *, int *);
void chequear_instrucciones(t_list* lista_instrucciones, int cant_instrucciones);
void loguear_instruccion(t_log* logger, t_instruccion i, int pid);

#endif /* INSTRUCCIONES_H_ */