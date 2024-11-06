#include "peticiones.h"

void* atender_peticion(t_buffer* buff){
    pthread_mutex_lock(&lock_peticiones);
    id_instruccion_fs codigo;
    int desplazamiento = 0;
    memcpy(&codigo, buff->stream, sizeof(id_instruccion_fs));
    desplazamiento += sizeof(id_instruccion_fs);
    int dir_fisica;
    char* id_archivo;
    int cant_bytes_a_acceder;
    int puntero_de_archivo;
    int longitud;
    int pid;
    void* valor;
    switch(codigo){

        case LEER_ARCHIVO:
        
            memcpy(&longitud, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            id_archivo = malloc(longitud);
            memcpy(id_archivo, buff->stream + desplazamiento, longitud);
            desplazamiento += longitud;
            memcpy(&puntero_de_archivo, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&cant_bytes_a_acceder, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&dir_fisica, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&pid, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(uint);
            //VER DE CAMBIAR EL FREE PARA QUE HAGA EL CONTROL DE LA LECTURA (CAPAZ)
            valor = leer_de_archivo(id_archivo, puntero_de_archivo, cant_bytes_a_acceder);
            escribir_en_memoria(dir_fisica, valor, cant_bytes_a_acceder, pid, socket_memoria);
            log_info(log_filesystem, "Leer Archivo: %s - Puntero: %d - Memoria: %d - Tamaño: %d", id_archivo, puntero_de_archivo, dir_fisica, cant_bytes_a_acceder);
            enviar_leer_escribir(id_archivo, LECTURA_FS_OK);
            free(id_archivo);
            free(valor);
            break;
        
        case ESCRIBIR_ARCHIVO:
            
            memcpy(&longitud, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            id_archivo = malloc(longitud);
            memcpy(id_archivo, buff->stream + desplazamiento, longitud);
            desplazamiento += longitud;
            memcpy(&puntero_de_archivo, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&cant_bytes_a_acceder, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&dir_fisica, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&pid, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            //DESCOMENTAR PARA CHECKPOINT 4 Y RESOLVER LA ESCRITURA
            valor = leer_de_memoria(dir_fisica, cant_bytes_a_acceder, pid, socket_memoria);
            escribir_en_archivo(id_archivo, puntero_de_archivo, valor, cant_bytes_a_acceder);
            sincronizar_bloques();
            log_info(log_filesystem, "Escribir Archivo: %s - Puntero: %d - Memoria: %d - Tamaño: %d",id_archivo,puntero_de_archivo,dir_fisica,cant_bytes_a_acceder);
            enviar_leer_escribir(id_archivo, ESCRITURA_FS_OK);
            free(id_archivo);
            break;
        
        case TRUNCAR:
            memcpy(&longitud, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            id_archivo = malloc(longitud);
            memcpy(id_archivo, buff->stream + desplazamiento, longitud);
            desplazamiento += longitud;
            memcpy(&longitud, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            longitud = truncar_fcb(id_archivo, longitud);
            sincronizar_bitmap();
            sincronizar_bloques();
            if(longitud >= 0) log_info(log_filesystem, "Truncar Archivo: %s - Tamaño: %d", id_archivo, longitud);
            enviar_truncamiento(id_archivo, longitud);
            free(id_archivo);
            break;

        case ABRIR:
            memcpy(&longitud, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            id_archivo = malloc(longitud);
            memcpy(id_archivo, buff->stream + desplazamiento, longitud);
            desplazamiento += longitud;
            if(existe_fcb(id_archivo))
                enviar_apertura(getFcb(id_archivo)->fcb->tam);                                               
            else
                enviar_apertura(-1);
            log_info(log_filesystem, "Abrir Archivo: %s", id_archivo);
            free(id_archivo);
            break;
        
        case CREAR:
            memcpy(&longitud, buff->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            id_archivo = malloc(longitud);
            memcpy(id_archivo, buff->stream + desplazamiento, longitud);
            desplazamiento += longitud;            
            crear_fcb(id_archivo);
            log_info(log_filesystem, "Crear Archivo: %s", id_archivo);
            enviar_creacion();
            break;        
    }
    
    liberar_buffer(buff);
    pthread_mutex_unlock(&lock_peticiones);
    return NULL;    
}


void enviar_creacion(){
    t_paquete* paquete = crear_paquete(CREACION);
    agregar_a_paquete(paquete, "CREACION OK", 12);
    enviar_paquete(paquete, socket_kernel);
}

void enviar_leer_escribir(char* id_archivo, op_code codigo){
    t_paquete* paquete = crear_paquete(codigo);
    int longitud = strlen(id_archivo) + 1;
    agregar_a_paquete(paquete, &longitud, sizeof(int));
    agregar_a_paquete(paquete, id_archivo, longitud);
    enviar_paquete(paquete, socket_kernel);
}

void enviar_truncamiento(char* nombre, int tam){
    t_paquete* paquete = crear_paquete(TRUNCAMIENTO);
    int tam_nombre = strlen(nombre) + 1;
    agregar_a_paquete(paquete, &tam, sizeof(int));
    agregar_a_paquete(paquete, &tam_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre, tam_nombre);
    enviar_paquete(paquete, socket_kernel);
}

void enviar_apertura(int tam){
    t_paquete* paquete = crear_paquete(APERTURA);
    agregar_a_paquete(paquete, &tam, sizeof(int));
    enviar_paquete(paquete, socket_kernel);
}

bool existe_fcb(char* nombre_archivo){
    return getFcb(nombre_archivo) != NULL; 
}