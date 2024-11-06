#include "peticiones.h"

void inicializar_memoria(){
    memoria_usuario = malloc(tam_memoria());
    segmento_0 = (t_segmento*) malloc(sizeof(t_segmento));
    segmento_0->base = 0;
    segmento_0->tamanio = tam_segmento_0();
    t_hueco_libre* hueco = (t_hueco_libre*) malloc(sizeof(t_hueco_libre));
    hueco->inicio = tam_segmento_0();
    hueco->fin = tam_memoria() - 1;
    huecos_libres = list_create();
    tablas_de_segmentos = list_create();
    
    sem_wait(&huecos_libres_sem);
    list_add(huecos_libres, hueco);
    sem_post(&huecos_libres_sem);    
}


void realizar_peticion(t_buffer* buff, char* modulo, int socket_a_enviar){
    int direccion_fisica;
    int desplazamiento;
    int desplazamiento_buff = 0;
    id_instruccion_memoria id;
    memcpy(&id, buff->stream, sizeof(id_instruccion_memoria));
    desplazamiento_buff += sizeof(id_instruccion_memoria); 
    
    int pid;
    memcpy(&pid, buff->stream + desplazamiento_buff, sizeof(int));
    desplazamiento_buff += sizeof(int);
    t_tabla_de_segmentos* tabla;
         
    switch(id){
        case LEER:
            usleep(retardo_memoria()*1000);
            memcpy(&direccion_fisica, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);
            
            memcpy(&desplazamiento, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);

            char* lectura = malloc(desplazamiento + 1);
            log_info(log_memoria, "PID: %d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: %s", pid, direccion_fisica, desplazamiento, modulo);             
            sem_wait(&memoria_usuario_sem);
            memcpy(lectura, memoria_usuario + direccion_fisica, desplazamiento);
            sem_post(&memoria_usuario_sem);                                  
            lectura[desplazamiento] = '\0';
            enviar_mensaje(lectura, desplazamiento + 1, socket_a_enviar);
            free(lectura);
            break;

        case ESCRIBIR:
            usleep(retardo_memoria()*1000);
            memcpy(&direccion_fisica, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);
            memcpy(&desplazamiento, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);
            void * valor = malloc(desplazamiento + 1);
            memcpy(valor, buff->stream + desplazamiento_buff, desplazamiento);
            desplazamiento_buff += desplazamiento;
            
            log_info(log_memoria, "PID: %d - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %d - Origen: %s", pid, direccion_fisica, desplazamiento, modulo);                                     
            sem_wait(&memoria_usuario_sem);
            memcpy(memoria_usuario + direccion_fisica, valor, desplazamiento);  
            sem_post(&memoria_usuario_sem);                     
            enviar_mensaje("ESCRITURA OK", 13, socket_a_enviar);
            free(valor);
            break;
        
        case CREAR_SEGMENTO:
            int tam_seg;
            int id_seg;
            memcpy(&tam_seg, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);
            memcpy(&id_seg, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);
            int respuesta = crear_segmento(tam_seg, pid, id_seg);
            if(respuesta >= 0)
                log_info(log_memoria, "PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d", pid, id_seg, respuesta, tam_seg);          
            
            enviar_base(respuesta, socket_a_enviar);
            break;
        
        case ELIMINAR_SEGMENTO:
            int id_segmento;
            memcpy(&id_segmento, buff->stream + desplazamiento_buff, sizeof(int));
            desplazamiento_buff += sizeof(int);
            sem_wait(&tablas_de_segmentos_sem);
            tabla = list_get(tablas_de_segmentos, get_index_tabla(pid));
            sem_post(&tablas_de_segmentos_sem);
            log_info(log_memoria, "PID: %d - Eliminar Segmento: %d - Base: %d - TAMAÑO: %d", pid, id_segmento, tabla->segmentos[id_segmento]->base, tabla->segmentos[id_segmento]->tamanio);
            borrar_segmento(tabla, id_segmento);
            enviar_tabla_segs(tabla, socket_a_enviar, TABLA_SEGMENTOS_ACTUALIZADA);
            break;

        case COMPACTAR:
            log_info(log_memoria, "Solicitud de Compactación");
            compactacion();
            loguear_compactacion();
            usleep(retardo_compactacion()*1000);
            enviarTablasSegmentos(socket_a_enviar);            
            break;

        case CREAR_PROCESO:
            log_info(log_memoria, "Creación de Proceso PID: %d", pid);            
            tabla = crear_tabla_segs(cant_segmentos(), pid);
            tabla->segmentos[0] = segmento_0;
            sem_wait(&tablas_de_segmentos_sem);
            list_add_sorted(tablas_de_segmentos, tabla, &orden_ascendente); 
            sem_post(&tablas_de_segmentos_sem);         
            enviar_tabla_segs(tabla, socket_a_enviar, TABLA_SEGMENTOS);
            break;

        case ELIMINAR_PROCESO:
            log_info(log_memoria, "Eliminación de Proceso PID: %d", pid);
            tabla = list_remove(tablas_de_segmentos, get_index_tabla(pid));
            for(int i = 1; i < tabla->tam; i++){
                if(tabla->segmentos[i] == NULL)
                    continue;                                            
                actualizar_huecos(tabla->segmentos[i]->base, tabla->segmentos[i]->tamanio, LIBERA);                               
            }
            liberar_tabla_segs(tabla);
            enviar_mensaje("ELIMINACION PROC OK", 20, socket_a_enviar);
            break;
        
        default:
            break;
    }
    liberar_buffer(buff);    
}
void loguear_compactacion(){
    for(int i=0; i < list_size(tablas_de_segmentos); i++){
        t_tabla_de_segmentos * tabla = list_get(tablas_de_segmentos,i);
        for(int j=0; j < tabla->tam; j++){
            if(tabla ->segmentos[j]!= NULL)
                log_info(log_memoria,"PID: %d - Segmento: %d - Base: %d - Tamaño %d",
                tabla ->pid, j, tabla->segmentos[j]->base,tabla->segmentos[j]->tamanio);
        }
    }
}

