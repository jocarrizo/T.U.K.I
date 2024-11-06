#ifndef _LEER_CONFIG_H_
#define _LEER_CONFIG_H_
#include <commons/config.h>
extern t_config* config;


void crear_config(char* config_path);
void destruir_config();
char* ip_memoria();
char* puerto_memoria();
char* puerto_escucha();
int retardo_instruccion();
int tam_max_segmento();
#endif // !_LEER_CONFIG_H_