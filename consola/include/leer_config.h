#ifndef _LEER_CONFIG_H_
#define _LEER_CONFIG_H_
#include <commons/config.h>
extern t_config* config;

void crear_config(char* config_path);
void destruir_config();
char* ip_kernel(); 
char* puerto_kernel();
#endif // !_LEER_CONFIG_H_