#include "planificacion.h"
//TODO: LIBERAR TODOS LOS RECURSOS YA SEA SEMAFOROS, COLAS, LISTAS, LOGS, HILOS, REC GLOBALES
void inicializarColas()
{
    new_queue = queue_create();
    ready_list = list_create();
    exit_list = list_create();
    inicializarColasBloqueo();
    tabla_procesos_activos = list_create();
}

void inicializarColasBloqueo()
{
    int i;
    int largo = string_array_size(recursos_config());

    bloqueado_recurso = (t_queue **)malloc(sizeof(t_queue *) * largo);

    for (i = 0; i < largo; i++)
    {
        bloqueado_recurso[i] = queue_create();
    }
}

void inicializarSemaforos()
{
    sem_init(&new_sem, 0, 1);
    sem_init(&ready_sem, 0, 1);
    sem_init(&block_sem, 0, 1);
    sem_init(&exit_sem, 0, 1);
    sem_init(&eliminacion_proceso_sem, 0, 0);
    sem_init(&tabla_actualizada_sem, 0, 0);
    sem_init(&buff_memoria_sem, 0, 1);
    sem_init(&recepcion_tabla, 0, 0);
    sem_init(&recepcion_base, 0, 1);
    sem_init(&truncamiento_sem, 0, 0);
    sem_init(&creacion_sem, 0, 0);
    sem_init(&apertura_sem, 0, 0);
    sem_init(&tablas_actualizadas_sem,0,0);    
    sem_init(&buff_fs_sem, 0, 1);
    sem_init(&leer_sem,0 , 0);

    sem_init(&hay_op_FS_MEM,0,1);
    sem_init(&contador_op_FS_MEM,0,0);
    sem_init(&tabla_procesos_activos_sem,0,1);
    // contadores
    sem_init(&new_contador_sem, 0, 0);
    sem_init(&ready_contador_sem, 0, 0);
    sem_init(&grado_multiprogramacion_sem, 0, grado_multiprogramacion());

    sem_init(&cpu_ready, 0, 1);
    sem_init(&sock_libre, 0, 1);

    //  
    sem_init(&recepcion_tabla,0,0);
    sem_init(&recepcion_base,0,0);
    // Cronometro
    tiempo = temporal_create();

}

void asignarTareasAHilos()
{

    pthread_create(&hilo_ready, NULL, (void *)thread_ready, NULL);
    pthread_create(&hilo_exec, NULL, (void *)thread_exec, NULL);
    pthread_create(&hilo_exit, NULL, (void *)thread_exit, NULL);
    // pthread_create(&hilo_block, NULL, &thread_block, NULL);

    // pthread_setname_np(hilo_ready, "READY");
    // pthread_setname_np(hilo_exec, "EXEC");
    // pthread_setname_np(hilo_block, "BLOCK");
    // pthread_setname_np(hilo_exit, "EXIT");
}
/*****************************NEW**************************************/
void agregar_new(t_pcb *pcb)
{
    sem_wait(&new_sem);

    queue_push(new_queue, pcb);

    sem_post(&new_sem);
    sem_post(&new_contador_sem);

    log_info(log_kernel, "Se crea el proceso %d en NEW", pcb->contexto_ejecucion->id);
}

t_pcb *sacar_new()
{
    t_pcb *pcb;
    sem_wait(&new_contador_sem);
    sem_wait(&new_sem);

    pcb = queue_pop(new_queue);

    sem_post(&new_sem);

    return pcb;
}
/*****************************READY**************************************/
void new_a_ready(t_pcb *pcb)
{
    //  Es la version de agregar a ready teniendo en cuenta que
    //  tiene en cuenta el grado de multiprogramacion e inicializa el contador del proceso.
    sem_wait(&grado_multiprogramacion_sem);
    pcb->contexto_ejecucion->tabla_segmentos = asignar_memoria(pcb->contexto_ejecucion->id);
    pcb->tabla_de_archivos = list_create();
    agregar_ready(pcb);
    sem_wait(&tabla_procesos_activos_sem);
    list_add(tabla_procesos_activos,pcb);
    sem_post(&tabla_procesos_activos_sem);
    log_info(log_kernel, "PID: %d - Estado Anterior: NEW - Estado Actual: READY", pcb->contexto_ejecucion->id);
}

void agregar_ready(t_pcb *pcb)
{ // EXIT bajan grado multiprog
    sem_wait(&ready_sem);

    temporal_stop(tiempo);
    pcb->llegada_a_ready = temporal_gettime(tiempo);
    list_add(ready_list, pcb);
    temporal_resume(tiempo);

    sem_post(&ready_sem);
    char *lista_pids = string_new();
    string_append(&lista_pids, "[");
    t_list_iterator *iter = list_iterator_create(ready_list);
    // hasta aca es parte de region critica
    t_pcb *pcb_aux;
    while (list_iterator_has_next(iter))
    {
        pcb_aux = list_iterator_next(iter);
        string_append_with_format(&lista_pids, "%d,", pcb_aux->contexto_ejecucion->id);
    }
    list_iterator_destroy(iter);
    lista_pids[strlen(lista_pids) - 1] = ']';
    log_info(log_kernel, "Cola Ready %s: %s", algoritmo_planificacion(), lista_pids);
    free(lista_pids);

    sem_post(&ready_contador_sem);
}

t_pcb *sacar_ready()
{
    t_pcb *pcb_a_sacar = NULL;

    if (!strcmp("HRRN", algoritmo_planificacion())) // CORREGIR CZEKPOINT 3
    {
        sem_wait(&ready_contador_sem);
        sem_wait(&ready_sem);

        temporal_stop(tiempo);
        pcb_a_sacar = list_get_maximum(ready_list, (void *)maximo);
        list_remove_element(ready_list, pcb_a_sacar);
        temporal_resume(tiempo);

        sem_post(&ready_sem);
    }

    if (!strcmp("FIFO", algoritmo_planificacion()))
    {
        sem_wait(&ready_contador_sem);
        sem_wait(&ready_sem);
        pcb_a_sacar = list_remove(ready_list, 0);
        sem_post(&ready_sem);
    }

    return pcb_a_sacar;
}

t_pcb *maximo(t_pcb *A, t_pcb *B)
{
    if (hrrn_pcb(A) > hrrn_pcb(B))
        return A;

    return B;
}

double hrrn_pcb(t_pcb *pcb)
{
    double W = temporal_gettime(tiempo) - pcb->llegada_a_ready;
    return (double)((W + pcb->est_rafaga) / pcb->est_rafaga);
}
/*****************************BLOCK**************************************/
void agregar_block(t_pcb *pcb, int id)
{
    queue_push(bloqueado_recurso[id], pcb);
}

void sacar_block(int id)
{
    t_pcb *pcb_aux = queue_pop(bloqueado_recurso[id]);
    log_info(log_kernel, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY", pcb_aux->contexto_ejecucion->id);
    pcb_aux->tabla_de_recursos[id]++;
    agregar_ready(pcb_aux);
}

void *block_io(t_block *s_bloqueo)
{

    sleep(s_bloqueo->tiempo);
    log_info(log_kernel, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY", (s_bloqueo->pcb)->contexto_ejecucion->id);
    agregar_ready(s_bloqueo->pcb);
    free(s_bloqueo);
    return NULL;
}

void bloquear_io(t_pcb *pcb, int tiempo)
{
    t_block *s_bloqueo = (t_block *)malloc(sizeof(t_block));
    s_bloqueo->pcb = pcb;
    s_bloqueo->tiempo = tiempo;
    log_info(log_kernel, "PID: %d - Ejecuta IO: %d", pcb->contexto_ejecucion->id, tiempo);
    pthread_create(&hilo_bloqueo_io, NULL, (void *)block_io, s_bloqueo);
    pthread_detach(hilo_bloqueo_io);
}
/*****************************EXIT**************************************/
void agregar_exit(t_pcb *pcb, char motivo[])
{
    if (!strcmp(motivo, "SUCCESS"))
        pcb->finalizacion = TERMINAR_CONSOLA;
    else
        pcb->finalizacion = ERROR;

    log_info(log_kernel, "Finaliza el proceso %d - Motivo: %s", pcb->contexto_ejecucion->id, motivo);
    sem_wait(&exit_sem);

    list_add(exit_list, pcb);

    sem_post(&exit_sem);
    sem_post(&exit_contador_sem);
}

t_pcb *sacar_exit()
{
    t_pcb *pcb;

    sem_wait(&exit_contador_sem);
    sem_wait(&exit_sem);

    pcb = list_remove(exit_list, 0);

    sem_post(&exit_sem);

    return pcb;
}

char *aux_str(char *msj, t_pcb *pcb)
{
    string_append(&msj, string_itoa(pcb->contexto_ejecucion->id));
    string_append(&msj, ",");
    return msj;
}

/*****************************HILOS DE PLANIFICADORES**************************************/
void *thread_ready()
{

    while (proceda)
    {
        new_a_ready(sacar_new());
    }
    return NULL;
}

void *thread_exec()
{
    while (proceda)
    {
        sem_wait(&cpu_ready);
        pcb_exec = sacar_ready();
        log_info(log_kernel, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", id_pcb_actual());
        enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
    }
    return NULL;
}

void *thread_exit()
{
    t_pcb *pcb;
    while (proceda)
    {
        pcb = sacar_exit(); // sacar_exit ya corrobora si la lista de exit esta vacia.

        sem_wait(&tabla_procesos_activos_sem);
        list_remove_element(tabla_procesos_activos, pcb);
        sem_post(&tabla_procesos_activos_sem);
        
        liberar_recursos(pcb->tabla_de_recursos);
        liberar_memoria(pcb->contexto_ejecucion->id);
        liberar_tabla_archivos(&(pcb->tabla_de_archivos));
        sem_wait(&eliminacion_proceso_sem);
        send(pcb->fd, &(pcb->finalizacion), sizeof(op_code), 0);
        liberar_pcb(pcb);
        sem_post(&grado_multiprogramacion_sem);
    }
    return NULL;
}

void realizar_op(op_code cod_op)
{
    char *rec_pedido;
    int aux;
    switch (cod_op){

        case EXIT_:
            agregar_exit(pcb_exec, "SUCCESS");
            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
            sem_post(&cpu_ready);
            break;

        case YIELD_:
            reestimar_rafaga_proceso();
            agregar_ready(pcb_exec);
            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: READY", id_pcb_actual());
            sem_post(&cpu_ready);
            break;

        case SEG_FAULT:
            agregar_exit(pcb_exec, "SEG_FAULT");
            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
            sem_post(&cpu_ready);
            break;

        case WAIT_:
            rec_pedido = (char *)malloc(strlen((instruccion_actual())->parametros[0]) + 1);
            strcpy(rec_pedido, (instruccion_actual())->parametros[0]);

            if (!existeRecurso(rec_pedido)){
                agregar_exit(pcb_exec, "UNKNOWN_RESOURCE");
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                free(rec_pedido);
                sem_post(&cpu_ready);
                break;
            }

            if (!recursoDisponible(rec_pedido)){
                reestimar_rafaga_proceso();
                agregar_block(pcb_exec, indiceRecurso(rec_pedido));
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", id_pcb_actual());
                log_info(log_kernel, "PID: %d - Bloqueado por: %s", id_pcb_actual(), rec_pedido);
                free(rec_pedido);
                sem_post(&cpu_ready);
                break;
            }

            waitRecurso(rec_pedido);
            log_info(log_kernel, "PID: %d - Wait: %s- Instancias: %d", id_pcb_actual(), rec_pedido, (recursoAsociado(rec_pedido))->disponibles);
            free(rec_pedido);
            enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
            break;

        case SIGNAL_:
            rec_pedido = (char *)malloc(strlen((instruccion_actual())->parametros[0]) + 1);
            strcpy(rec_pedido, (instruccion_actual())->parametros[0]);

            if (!existeRecurso(rec_pedido)){
                agregar_exit(pcb_exec, "UNKNOWN_RESOURCE");
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                free(rec_pedido);
                sem_post(&cpu_ready);
                break;
            }
            
            signalRecurso(rec_pedido);

            if (!overflowDeRecursos(rec_pedido)){
                agregar_exit(pcb_exec, "OVERFLOW_RESOURCE");
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                waitRecurso(rec_pedido);
                free(rec_pedido);
                sem_post(&cpu_ready);
                break;
            }
            
            log_info(log_kernel, "PID: %d - Signal: %s - Instancias: %d", id_pcb_actual(), rec_pedido, (recursoAsociado(rec_pedido))->disponibles);
            free(rec_pedido);
            enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
            break;

        case IO_:
            int tiempoDeBloqueo = atoi((instruccion_actual())->parametros[0]);
            reestimar_rafaga_proceso();
            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", id_pcb_actual());
            log_info(log_kernel, "PID: %d - Bloqueado por: IO", id_pcb_actual());
            bloquear_io(pcb_exec, tiempoDeBloqueo);
            sem_post(&cpu_ready);
            break;

        case CREATE_SEGMENT_:
            log_info(log_kernel, "PID: %d- Crear Segmento - Id: %d - Tamaño: %d",id_pcb_actual(),id_segmento_pedido(),tamanio_segmento_pedido());

            pedir_segmento(tamanio_segmento_pedido(), id_pcb_actual(), id_segmento_pedido());
            int base = recibir_base();

            if (base == -2){
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                agregar_exit(pcb_exec, "OUT_OF_MEMORY");
                sem_post(&cpu_ready);
                break;
            }

            if (base == -1){
                log_info(log_kernel, "Compactación: Esperando Fin de Operaciones de FS");
                sem_wait(&hay_op_FS_MEM);
                log_info(log_kernel, "Compactación: Se solicitó compactación");
                pedir_compactacion();
                log_info(log_kernel, "Se finalizó el proceso de compactación");
                sem_post(&hay_op_FS_MEM);

                pedir_segmento(tamanio_segmento_pedido(), id_pcb_actual(), id_segmento_pedido());
                base = recibir_base();
            }

            asignar_segmento(base);

            enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
            break;

        case DELETE_SEGMENT_:
            log_info(log_kernel, "PID: %d - Eliminar Segmento - Id Segmento: %d",id_pcb_actual(),id_segmento_pedido());

            eliminar_segmento(id_pcb_actual(), id_segmento_pedido());

            enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
            break;

        case F_OPEN_:
            log_info(log_kernel, "PID: %d - Abrir Archivo: %s",id_pcb_actual(),(instruccion_actual())->parametros[0]);

            char* archivo = (char *)malloc(strlen((instruccion_actual())->parametros[0]) + 1);
            strcpy(archivo, (instruccion_actual())->parametros[0]);
            if (corroborar_tablaglobal_archivos(archivo)){                
                agregar_a_tablaArchivosP(archivo);                
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", id_pcb_actual());
                log_info(log_kernel, "PID: %d - Bloqueado por: %s", id_pcb_actual(), archivo);
                agregar_a_procesosBlock(archivo);                
                sem_post(&cpu_ready);
            }
            else{
                abrir_archivo(archivo);
                enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);                
            }
            
            free(archivo);      
            break;

        case F_CLOSE_:
            if(!checkear_tabla_archivos_proceso(instruccion_actual()->parametros[0])){
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                agregar_exit(pcb_exec, "UNKNOWN_FILE");
                sem_post(&cpu_ready);
                return;
            }
            log_info(log_kernel, "PID: %d - Cerrar Archivo: %s",id_pcb_actual(),(instruccion_actual())->parametros[0]);
            
            cerrar_archivo(instruccion_actual()->parametros[0]);            
            enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
            break;

        case F_SEEK_:
            if(!checkear_tabla_archivos_proceso(instruccion_actual()->parametros[0])){
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                agregar_exit(pcb_exec, "UNKNOWN_FILE");
                sem_post(&cpu_ready);
                return;
            }

            log_info(log_kernel, "PID: %d - Actualizar puntero Archivo: %s - Puntero %d",id_pcb_actual(),(instruccion_actual())->parametros[0], atoi((instruccion_actual())->parametros[1]));
            actualizar_puntero((instruccion_actual()->parametros[0]), atoi((instruccion_actual()->parametros[1])));
            enviar_contexto(pcb_exec->contexto_ejecucion, socket_cpu, CONTEXTO_EJECUCION);
            break;

        case F_TRUNCATE_:
            if(!checkear_tabla_archivos_proceso(instruccion_actual()->parametros[0])){
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                agregar_exit(pcb_exec, "UNKNOWN_FILE");
                sem_post(&cpu_ready);
                return;
            }

            log_info(log_kernel, "PID: %d - Archivo: %s - Tamaño: %s",id_pcb_actual(),(instruccion_actual())->parametros[0],(instruccion_actual())->parametros[1]);
            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", id_pcb_actual());
            log_info(log_kernel, "PID: %d - Bloqueado por: %s", id_pcb_actual(), instruccion_actual()->parametros[0]);
            actualizar_tamanio(instruccion_actual()->parametros[0], atoi(instruccion_actual()->parametros[1]));
            sem_post(&cpu_ready);
            pthread_create(&hilo_bloqueo_fs, NULL, (void*)esperar_truncamiento, NULL);
            pthread_detach(hilo_bloqueo_fs);
            break;

        case F_READ_:
            if(!checkear_tabla_archivos_proceso(instruccion_actual()->parametros[0])){
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                agregar_exit(pcb_exec, "UNKNOWN_FILE");
                sem_post(&cpu_ready);
                return;
            }
            
            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", id_pcb_actual());
            log_info(log_kernel, "PID: %d - Bloqueado por: %s", id_pcb_actual(), instruccion_actual()->parametros[0]);
            log_info(log_kernel, "PID: %d- Leer Archivo: %s - Puntero %d - Dirección Memoria %d - Tamaño %s", id_pcb_actual(),(instruccion_actual())->parametros[0], buscar_puntero(instruccion_actual()->parametros[0]), dir_fisica, instruccion_actual()->parametros[2]);
            
            sem_getvalue(&hay_op_FS_MEM,&aux);
            if(aux == 1) sem_wait(&hay_op_FS_MEM);
            sem_post(&contador_op_FS_MEM);
            
            leer_escribir_fs(instruccion_actual()->parametros[0], buscar_puntero(instruccion_actual()->parametros[0]), atoi(instruccion_actual()->parametros[2]), dir_fisica, LEER_ARCHIVO);
            sem_post(&cpu_ready);
            pthread_create(&hilo_bloqueo_fs, NULL, (void*)esperar_ok_fs, &leer_sem);
            pthread_detach(hilo_bloqueo_fs);
            break;

        case F_WRITE_:
            if(!checkear_tabla_archivos_proceso(instruccion_actual()->parametros[0])){
                log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", id_pcb_actual());
                agregar_exit(pcb_exec, "UNKNOWN_FILE");
                sem_post(&cpu_ready);
                return;
            }

            log_info(log_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", id_pcb_actual());
            log_info(log_kernel, "PID: %d - Bloqueado por: %s", id_pcb_actual(), instruccion_actual()->parametros[0]);
            log_info(log_kernel, "PID: %d- Escribir Archivo: %s - Puntero %d - Dirección Memoria %d - Tamaño %s", id_pcb_actual(),(instruccion_actual())->parametros[0], buscar_puntero(instruccion_actual()->parametros[0]), dir_fisica, instruccion_actual()->parametros[2]);
        
            sem_getvalue(&hay_op_FS_MEM,&aux);
            if(aux == 1) sem_wait(&hay_op_FS_MEM);
            sem_post(&contador_op_FS_MEM);

            leer_escribir_fs(instruccion_actual()->parametros[0], buscar_puntero(instruccion_actual()->parametros[0]), atoi(instruccion_actual()->parametros[2]), dir_fisica, ESCRIBIR_ARCHIVO);
            sem_post(&cpu_ready);
            pthread_create(&hilo_bloqueo_fs, NULL, (void*)esperar_ok_fs, &escribir_sem);
            pthread_detach(hilo_bloqueo_fs);
            break;

        default:;
    }
}

int buscar_puntero(char* archivo){
    for (int i=0; i<list_size(pcb_exec->tabla_de_archivos);i++){
        t_tabla_archivos_proc* archivoActual = list_get(pcb_exec->tabla_de_archivos, i);
        if(strcmp(archivoActual->fcb->nombre, archivo) == 0)
            return archivoActual->puntero;           
    }
    return -1; 
}

void reestimar_rafaga_proceso(){
    pcb_exec->est_rafaga = pcb_exec->est_rafaga * alfa() + (1 - alfa()) * pcb_exec->contexto_ejecucion->tiempo_real_ej;
}

t_instruccion *instruccion_actual(){
    int posicion = pcb_exec->contexto_ejecucion->program_counter - 1;
    return list_get(pcb_exec->contexto_ejecucion->instrucciones, posicion);
}

int tamanio_segmento_pedido(){
    return atoi((instruccion_actual())->parametros[1]);
}

int id_segmento_pedido(){
    return atoi((instruccion_actual())->parametros[0]);
}

int id_pcb_actual(){
    return pcb_exec->contexto_ejecucion->id;
}

void* esperar_truncamiento(){
    sem_wait(&truncamiento_sem);
    int truncamiento;
    int desplazamiento = 0;
    int largo_archivo;
    char* archivo;
    t_archivo* archivo_tabla;

    memcpy(&truncamiento, buff_fs->stream, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&largo_archivo, buff_fs->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    archivo = malloc(largo_archivo);
    memcpy(archivo, buff_fs->stream + desplazamiento, largo_archivo);
    liberar_buffer(buff_fs);
    sem_post(&buff_fs_sem);

    archivo_tabla = archivo_asociado(archivo);
    if(truncamiento < 0){
        log_info(log_kernel, "PID: %d - Estado Anterior: BLOCK - Estado Actual: EXIT", archivo_tabla->proceso_lock->contexto_ejecucion->id);
        agregar_exit(archivo_tabla->proceso_lock, "OUT OF DISK SPACE");
        return NULL;
    }
        
    archivo_tabla->fcb->tamanio = truncamiento;
    log_info(log_kernel, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY", archivo_tabla->proceso_lock->contexto_ejecucion->id);
    agregar_ready(archivo_tabla->proceso_lock);
    free(archivo);
    
    return NULL;
}

void* esperar_ok_fs(void* sem){
    sem_wait(sem);
    sem_wait(&contador_op_FS_MEM);
    int desplazamiento = 0;
    int largo_archivo;
    char* archivo;
    int valor_contador;
    t_archivo* archivo_tabla;
    memcpy(&largo_archivo, buff_fs->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    archivo = malloc(largo_archivo);
    memcpy(archivo, buff_fs->stream + desplazamiento, largo_archivo);
    liberar_buffer(buff_fs);
    sem_post(&buff_fs_sem);
    archivo_tabla = archivo_asociado(archivo);
    log_info(log_kernel, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY", archivo_tabla->proceso_lock->contexto_ejecucion->id);
    agregar_ready(archivo_tabla->proceso_lock);
    free(archivo);
    sem_getvalue(&contador_op_FS_MEM, &valor_contador);
    if(valor_contador == 0)
        sem_post(&hay_op_FS_MEM);

    return NULL;
}

void sacar_de_procesos_block(t_archivo* entrada){
   
    if(list_size(entrada->procesos_block) == 0){
        list_remove_element(tabla_global_archivos_abiertos, entrada);
        remover_entrada_de_tabla_global(entrada);
        return;
    }

    
    entrada->proceso_lock = (t_pcb* )list_remove(entrada->procesos_block, 0);
    log_info(log_kernel, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY",entrada->proceso_lock->contexto_ejecucion->id);
    agregar_ready(entrada->proceso_lock);
}

void liberar_tabla_archivos(t_list** tabla){
    for(int i = 0; i < list_size(*tabla); i++)
        sacar_de_procesos_block(archivo_asociado(((t_tabla_archivos_proc*) list_get(*tabla, i))->fcb->nombre));    
        
    list_destroy_and_destroy_elements(*tabla, free);
}

void cerrar_archivo(char* nombre){
    t_tabla_archivos_proc* archivo = NULL;
    for (int i = 0; i<list_size(pcb_exec->tabla_de_archivos); i++){
        archivo = list_get(pcb_exec->tabla_de_archivos,i);
        if(strcmp(archivo->fcb->nombre, nombre)== 0 ) {
            list_remove(pcb_exec->tabla_de_archivos,i);
            break;
        }        
    }
    t_archivo* archivoGlobal = archivo_asociado(archivo->fcb->nombre);
    free(archivo);    
    sacar_de_procesos_block(archivoGlobal);        
}

