#ifndef _LEER_CONFIG_H_
#define _LEER_CONFIG_H_
#include <commons/config.h>
extern t_config* config;


void crear_config(char* config_path);
void destruir_config();
char* puerto_escucha();
int tam_memoria();
int tam_segmento_0();
int cant_segmentos();
int retardo_memoria();
int retardo_compactacion();
char* algoritmo_asignacion();
#endif // !_LEER_CONFIG_H_