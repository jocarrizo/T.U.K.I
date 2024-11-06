#ifndef INSTRUCCIONES_MEMORIA_H_
#define INSTRUCCIONES_MEMORIA_H_
#include "instrucciones/instrucciones.h"
#include "sockets/sockets.h"
#include <commons/log.h>
typedef enum{
    LEER, ESCRIBIR, COMPACTAR, CREAR_SEGMENTO, ELIMINAR_SEGMENTO, CREAR_PROCESO, ELIMINAR_PROCESO
}id_instruccion_memoria; 

char* leer_de_memoria(int dir_fisica, int cant_bytes, int pid, int socket_memoria);
void escribir_en_memoria(int dir_fisica, void* valor, int cant_bytes, int pid, int socket_memoria);

#endif