#include <peticiones_memoria.h>

t_tabla_de_segmentos* asignar_memoria(int pid){
    t_tabla_de_segmentos* tabla;
    t_paquete* a_enviar =  crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = CREAR_PROCESO;
    agregar_a_paquete(a_enviar, &codigo, sizeof(id_instruccion_memoria));
    agregar_a_paquete(a_enviar, &pid, sizeof(int));
    enviar_paquete(a_enviar, socket_memoria);

    sem_wait(&recepcion_tabla);  
    int desplazamiento = 0;    
    tabla = desempaquetar_tabla_segs(buff_memoria->stream, &desplazamiento);
    liberar_buffer(buff_memoria);
    sem_post(&buff_memoria_sem); 
    
    return tabla;
}

void pedir_segmento(int tamanio,int pid,int id_seg){
    t_paquete* a_enviar =  crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = CREAR_SEGMENTO;
    agregar_a_paquete(a_enviar, &codigo, sizeof(id_instruccion_memoria));
    agregar_a_paquete(a_enviar, &pid, sizeof(int));
    agregar_a_paquete(a_enviar, &tamanio, sizeof(int));
    agregar_a_paquete(a_enviar, &id_seg, sizeof(int));
    
    enviar_paquete(a_enviar, socket_memoria); 
}

int recibir_base(){
    sem_wait(&recepcion_base);
    int _base;

    memcpy(&(_base), buff_memoria->stream, sizeof(int));
    liberar_buffer(buff_memoria);
    sem_post(&buff_memoria_sem);
    return _base;
}

void asignar_segmento(int _base){
    t_segmento* seg = (t_segmento*) malloc(sizeof(t_segmento));
    seg->base = _base;
    seg->tamanio = tamanio_segmento_pedido();

    pcb_exec->contexto_ejecucion->tabla_segmentos->segmentos[id_segmento_pedido()] = seg;
}

void pedir_compactacion(){
    t_paquete* a_enviar =  crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = COMPACTAR;
    agregar_a_paquete(a_enviar, &codigo, sizeof(id_instruccion_memoria));
    enviar_paquete(a_enviar, socket_memoria); 

    sem_wait(&tablas_actualizadas_sem);
    t_tabla_de_segmentos* tabla;
    int desplazamiento = 0;
    //TODO: ver si se actualiza desplazamiento
    sem_wait(&tabla_procesos_activos_sem);
    for(int i=0; i< list_size(tabla_procesos_activos);i++){
        tabla = desempaquetar_tabla_segs(buff_memoria->stream,&desplazamiento);
        actualizar_tabla(tabla);
    }
    sem_post(&tabla_procesos_activos_sem);
    liberar_buffer(buff_memoria);
    sem_post(&buff_memoria_sem);
}

void actualizar_tabla(t_tabla_de_segmentos* tabla){

    for (int i = 0; i < list_size(tabla_procesos_activos); i++) {
        t_pcb* actual = list_get(tabla_procesos_activos, i);
        if (tabla->pid == actual->contexto_ejecucion->id) {
            liberar_tabla_segs_completa(actual->contexto_ejecucion->tabla_segmentos);
            actual->contexto_ejecucion->tabla_segmentos = tabla;
            return;
        }
    }
}

void eliminar_segmento(int pid, int id_seg){
    t_paquete* a_enviar =  crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = ELIMINAR_SEGMENTO;
    agregar_a_paquete(a_enviar, &codigo, sizeof(id_instruccion_memoria));
    agregar_a_paquete(a_enviar, &pid, sizeof(int));
    agregar_a_paquete(a_enviar, &id_seg, sizeof(int));

    enviar_paquete(a_enviar, socket_memoria);
    sem_wait(&tabla_actualizada_sem);
    asignar_tabla_actualizada();
    liberar_buffer(buff_memoria);
    sem_post(&buff_memoria_sem);
}

void asignar_tabla_actualizada(){

    int desplazamiento = 0;
    liberar_tabla_segs_completa(pcb_exec->contexto_ejecucion->tabla_segmentos);
    pcb_exec->contexto_ejecucion->tabla_segmentos = desempaquetar_tabla_segs(buff_memoria->stream, &desplazamiento);
}

void liberar_memoria(int pid){
    t_paquete* a_enviar =  crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = ELIMINAR_PROCESO;
    agregar_a_paquete(a_enviar, &codigo, sizeof(id_instruccion_memoria));
    agregar_a_paquete(a_enviar, &pid, sizeof(int));

    enviar_paquete(a_enviar, socket_memoria); 
}