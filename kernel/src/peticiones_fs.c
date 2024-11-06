#include <peticiones_fs.h>

void inicializar_tabladearchivos_globales(){

    tabla_global_archivos_abiertos=list_create();

}
bool corroborar_tablaglobal_archivos(char*archivo){
    
    for (int i=0; i<list_size(tabla_global_archivos_abiertos);i++){
        t_archivo* archivoActual = list_get(tabla_global_archivos_abiertos, i);
        if(strcmp(archivoActual->fcb->nombre, archivo) == 0) return true;
    }
   
    return false;
}

void agregar_a_tablaArchivosP(char* archivo){
    t_archivo* archivoActual = archivo_asociado(archivo);
    t_tabla_archivos_proc* entrada = (t_tabla_archivos_proc*) malloc(sizeof(t_tabla_archivos_proc));
    entrada->puntero = 0;
    entrada->fcb = archivoActual->fcb;
    list_add(pcb_exec->tabla_de_archivos, entrada);        
}

t_archivo* archivo_asociado(char* archivo){
    t_archivo* archivoActual = NULL;
    for (int i=0; i<list_size(tabla_global_archivos_abiertos);i++){
        archivoActual = list_get(tabla_global_archivos_abiertos, i);
        if(strcmp(archivoActual->fcb->nombre, archivo) == 0) break;    
    }
    return archivoActual;
}
void agregar_a_procesosBlock(char* archivo){
    t_archivo* archivo_abierto = archivo_asociado(archivo);
    list_add(archivo_abierto->procesos_block, pcb_exec);
}

void agregar_archivo_tabla_global(char* archivo, int tam){
    int largo_nombre = strlen(archivo) + 1;
    t_archivo* nueva_entrada = (t_archivo*) malloc(sizeof(t_archivo));
    nueva_entrada->fcb = (t_fcb_kernel*) malloc(sizeof(t_fcb_kernel));
    nueva_entrada->fcb->nombre = (char*) malloc(sizeof(char) * largo_nombre);
    memcpy(nueva_entrada->fcb->nombre, archivo, largo_nombre);
    nueva_entrada->fcb->tamanio = tam;
    nueva_entrada->proceso_lock = pcb_exec;
    nueva_entrada->procesos_block = list_create();
    list_add(tabla_global_archivos_abiertos, nueva_entrada);
}

void abrir_archivo(char* archivo){
    t_paquete* paquete = crear_paquete(INSTRUCCIONES_FS);
    int largo = strlen(archivo) + 1;
    id_instruccion_fs codigo = ABRIR;
    agregar_a_paquete(paquete, &codigo, sizeof(id_instruccion_fs));
    agregar_a_paquete(paquete, &largo, sizeof(int));
    agregar_a_paquete(paquete, archivo, largo);
    enviar_paquete(paquete, socket_fs);

    sem_wait(&apertura_sem);
    int respuesta;
    memcpy(&respuesta, buff_fs->stream, sizeof(int));
    if(respuesta >= 0){
        agregar_archivo_tabla_global(archivo, respuesta);
        agregar_a_tablaArchivosP(archivo);
        liberar_buffer(buff_fs);
        sem_post(&buff_fs_sem);
        return;
    }
    liberar_buffer(buff_fs);
    sem_post(&buff_fs_sem);
    paquete = crear_paquete(INSTRUCCIONES_FS);
    codigo = CREAR;
    agregar_a_paquete(paquete, &codigo, sizeof(id_instruccion_fs));
    agregar_a_paquete(paquete, &largo, sizeof(int));
    agregar_a_paquete(paquete, archivo, largo);
    enviar_paquete(paquete, socket_fs);

    sem_wait(&creacion_sem);
    if(strcmp("CREACION OK", buff_fs->stream) == 0){
        agregar_archivo_tabla_global(archivo, 0);
        agregar_a_tablaArchivosP(archivo);
    }
        
    /*

    else{
        //gestionar el error
    }
    */  
    liberar_buffer(buff_fs);
    sem_post(&buff_fs_sem);
    
}

void actualizar_tamanio(char* archivo, int tamanioNuevo){

    t_paquete* paquete= crear_paquete(INSTRUCCIONES_FS);
    int largo = strlen(archivo) + 1;
    id_instruccion_fs codigo = TRUNCAR;
    agregar_a_paquete(paquete, &codigo, sizeof(id_instruccion_fs));
    agregar_a_paquete(paquete, &largo, sizeof(int));//avisando el largo del nombre para guardar 
    agregar_a_paquete(paquete, archivo, largo );// le mando el nombre 
    agregar_a_paquete(paquete, &tamanioNuevo, sizeof(int));
    enviar_paquete(paquete, socket_fs);    
}

bool checkear_tabla_archivos_proceso(char* nombre_archivo){
    if(list_size(pcb_exec->tabla_de_archivos) == 0)
        return false;
        
    t_list_iterator* iterador = list_iterator_create(pcb_exec->tabla_de_archivos);
    t_tabla_archivos_proc* siguiente = NULL;
    while(list_iterator_has_next(iterador)){
        siguiente = list_iterator_next(iterador);
        if(strcmp(siguiente->fcb->nombre, nombre_archivo) == 0){
            list_iterator_destroy(iterador);
            return true;
        }
    }
    if(strcmp(siguiente->fcb->nombre, nombre_archivo) == 0){
        list_iterator_destroy(iterador);
        return true;
    }

    list_iterator_destroy(iterador);
    return false;
}



void leer_escribir_fs(char* archivo, int puntero, int cantidadBytes, int direccionFisica, id_instruccion_fs operacion){
    t_paquete* paquete = crear_paquete(INSTRUCCIONES_FS);
    int largo = strlen(archivo) + 1;
    id_instruccion_fs codigo = operacion;
    agregar_a_paquete(paquete, &codigo, sizeof(id_instruccion_fs));
    agregar_a_paquete(paquete, &largo, sizeof(int));
    agregar_a_paquete(paquete, archivo, largo );
    agregar_a_paquete(paquete, &puntero, sizeof(int));
    agregar_a_paquete(paquete, &cantidadBytes, sizeof(int));
    agregar_a_paquete(paquete, &direccionFisica, sizeof(int));
    agregar_a_paquete(paquete, &pcb_exec->contexto_ejecucion->id, sizeof(uint));
    enviar_paquete(paquete, socket_fs);
}

void actualizar_puntero (char* archivo, int puntero){

    for (int i=0; i<list_size(pcb_exec->tabla_de_archivos);i++){
        t_tabla_archivos_proc* archivoActual = list_get(pcb_exec->tabla_de_archivos, i);
        if(strcmp(archivoActual->fcb->nombre, archivo) == 0){
            archivoActual->puntero = puntero; 
            return;
        }   
    }  
}

void remover_entrada_de_tabla_global(void* entrada){    
    list_destroy(((t_archivo*)entrada)->procesos_block);
    free(((t_archivo*)entrada)->fcb->nombre);
    free(((t_archivo*)entrada)->fcb);
    free(((t_archivo*)entrada));
}