#ifndef _LEER_CONFIG_H_
#define _LEER_CONFIG_H_
#include <commons/config.h>
extern t_config* config;


void crear_config(char* config_path);
void destruir_config();

//memoria
char *ip_memoria();
char *puerto_memoria();

//cpu
char *ip_cpu();
char *puerto_cpu();

//filesystem
char *ip_filesystem();
char *puerto_filesystem();

//kernel
char *puerto_escucha();

//planificacion
char *algoritmo_planificacion();
int grado_multiprogramacion();

double estimacion_inicial();
double alfa();

// recursos
char **recursos_config();

char **instancias_recursos();

#endif // !_LEER_CONFIG_H_
