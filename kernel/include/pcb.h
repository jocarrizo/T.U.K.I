#ifndef _PCB_H_
#define _PCB_H_
#include <contexto_ejecucion/contexto_ejecucion.h>

typedef struct{
  char*nombre;  
  int tamanio;
}t_fcb_kernel;

typedef struct{
    t_fcb_kernel* fcb;
	int puntero;
}t_tabla_archivos_proc;

typedef struct {
	int fd; 
	t_contexto_ejecucion* contexto_ejecucion;
	double est_rafaga;
	double llegada_a_ready;			
	t_list* tabla_de_archivos; 		// ver si el tipo es int y como es todo el tema de la tabla 
	int* tabla_de_recursos;			// recursos asignados
	op_code finalizacion;
} t_pcb;

void imprimir_pcb(t_pcb *pcb);
void liberar_pcb(t_pcb *pcb);

#endif // !_PCB_H_