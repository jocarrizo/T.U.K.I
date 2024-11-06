#ifndef _RECURSOS_H_
#define _RECURSOS_H_
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <commons/collections/list.h>
#include <commons/string.h>

#include <leer_config.h>
#include <planificacion.h>

typedef struct {
    char* nombre;
    int cantidad;
    int disponibles;
}t_recurso;

extern t_list* recursos;

void crearListaDeRecursos();
t_recurso* recursoAsociado(char* rec);
bool existeRecurso(char* rec);
int indiceRecurso(char* rec);
bool recursoDisponible(char* rec);
void waitRecurso(char* rec);
void signalRecurso(char* rec);
bool overflowDeRecursos(char* rec);
void eliminarListaDeRecursos();
void liberarNombre(t_recurso* rec);
void liberar_recursos(int *tabla_de_recursos);
void remover_recurso(void*);
#endif // !_RECURSOS_H_
