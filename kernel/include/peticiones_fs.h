#ifndef _PETICIONES_FS_H_
#define _PETICIONES_FS_H_

#include <commons/collections/list.h>
#include <pcb.h>
#include <sockets/sockets.h>
#include <semaphore.h>
#include <instrucciones_fs/instrucciones_fs.h>

typedef struct{
    t_fcb_kernel* fcb;
    t_list* procesos_block;
    t_pcb* proceso_lock;
}t_archivo;

extern t_list* tabla_global_archivos_abiertos;
extern t_pcb* pcb_exec; 
extern int socket_fs; 
extern t_buffer* buff_fs;
extern sem_t buff_fs_sem;
extern sem_t apertura_sem;
extern sem_t creacion_sem;
extern sem_t truncamiento_sem;
extern sem_t cpu_ready;
extern sem_t leer_sem;
extern sem_t escribir_sem;

bool corroborar_tablaglobal_archivos(char*);
void abrir_archivo(char*);
void inicializar_tabladearchivos_globales();
void agregar_a_tablaArchivosP(char*);
t_archivo* archivo_asociado(char*);
void agregar_a_procesosBlock(char*);
void agregar_archivo_tabla_global(char*, int);
void actualizar_tamanio(char*, int);
bool checkear_tabla_archivos_proceso(char*);
void actualizar_puntero (char*, int);
void leer_escribir_fs(char*, int, int, int, id_instruccion_fs);
void remover_entrada_de_tabla_global(void*);

#endif