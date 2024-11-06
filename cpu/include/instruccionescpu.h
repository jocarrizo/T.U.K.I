#ifndef _INSTRUCCIONES_CPU_H_
#define _INSTRUCCIONES_CPU_H_
#include <contexto_ejecucion/contexto_ejecucion.h>
#include <commons/log.h>
#include <instrucciones_memoria/instrucciones_memoria.h>
#include <leer_config.h>

extern int socket_memoria;
extern int socket_kernel;
extern int direccion_fisica;
extern int desplazamiento_segmento;
extern int num_segmento;

extern t_log* log_cpu;
extern t_config* config;
extern t_registros regs;
extern t_temporal* tiempo_real_ejecucion;
extern t_segmento* seg_trabajo;

void ciclo_de_instruccion(t_contexto_ejecucion* contexto);
t_instruccion fetch(t_contexto_ejecucion* contexto);
bool decode(t_instruccion instruccion);
void execute(t_instruccion instruccion_actual, t_contexto_ejecucion** contexto);
void enviar_contexto_read_write(t_contexto_ejecucion*, int, op_code, int);

#endif // !_INSTRUCCIONES_CPU_H_