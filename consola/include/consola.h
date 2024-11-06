#ifndef _CONSOLA_H_
#define _CONSOLA_H_
#include <stdio.h>
#include <stdlib.h>
#include <conexion_kernel.h>
#include <crear_paquete.h>

t_log *log_consola;
t_config* config;

int id_consola = 9;

FILE *son_argumentos_validos(int cantidad_argumentos, char **argv);
#endif // !_CONSOLA_H_