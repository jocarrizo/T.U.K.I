#include <recursos.h>

/*****************************MANEJO DE RECURSOS**************************************/

void crearListaDeRecursos(){
    char** recursos_aux = recursos_config();
    char** instancias = instancias_recursos();
    int tamanio = string_array_size(recursos_aux);
    recursos = list_create();
    
    //Inserta recurso a la lista de recursos.
    for (int i = 0; i < tamanio; i++) {
        t_recurso* aux = malloc(sizeof(t_recurso));
        aux->nombre = malloc(sizeof(char*));

        aux->nombre = recursos_aux[i];
        aux->cantidad = atoi(instancias[i]);
        aux->disponibles = aux->cantidad;        
        list_add(recursos, aux);
    }
    free(recursos_aux);
    free(instancias);
}

t_recurso* recursoAsociado(char* rec){
    return list_get(recursos, indiceRecurso(rec));
}

bool existeRecurso(char* rec) {
    bool found = false;
    int tamanio = list_size(recursos);

    for (int i = 0; i < tamanio; i++) {
        t_recurso* actual = list_get(recursos, i);
        if (string_equals_ignore_case(actual->nombre, rec)) {
            found = true;
            break;
        }
    }
    return found;
}

int indiceRecurso(char* rec) {
    for (int i = 0; i < list_size(recursos); i++) {
        t_recurso* actual = list_get(recursos, i);
        if (string_equals_ignore_case(actual->nombre, rec)) {
            return i;
        }
    }
    //error
    return -1;
}

bool recursoDisponible(char* rec){
    t_recurso* aux = recursoAsociado(rec);

    return aux->disponibles > 0;
}

void waitRecurso(char* rec){
    t_recurso* aux = recursoAsociado(rec);

    pcb_exec->tabla_de_recursos[indiceRecurso(rec)]++;
    aux->disponibles--;
}

void signalRecurso(char* rec){
    t_recurso* aux = recursoAsociado(rec);
    //if(pcb_exec->tabla_de_recursos[indiceRecurso(rec)] > 0)
        pcb_exec->tabla_de_recursos[indiceRecurso(rec)]--;
    
    if(!queue_is_empty(bloqueado_recurso[indiceRecurso(rec)]))
        sacar_block(indiceRecurso(rec));
    else
        aux->disponibles++;

}

bool overflowDeRecursos(char* rec){
    t_recurso* aux = recursoAsociado(rec);

    return aux->cantidad >= aux->disponibles;
}

void eliminarListaDeRecursos(){
    list_destroy_and_destroy_elements(recursos, remover_recurso);
}

void liberarNombre(t_recurso* rec){
    free(rec->nombre);
}

void liberar_recursos(int *recursos_de_pcb){
    t_recurso* aux;
    for(int i = 0 ; i<list_size(recursos) ; i++){
        aux  = list_get(recursos,i);
        aux->disponibles += recursos_de_pcb[i];
    }
    free(recursos_de_pcb);
}

void remover_recurso(void* rec){
    free(((t_recurso*)rec)->nombre);
    free(((t_recurso*)rec));
}