#ifndef _LEER_CONFIG_H_
#define _LEER_CONFIG_H_
#include <commons/config.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <commons/log.h>
#include <fcb/fcb.h>

typedef struct{
    t_fcb* fcb;
    t_config* fcb_config;
}t_fcb_filesystem;

extern t_config* config;
extern t_config* config_superbloque;
extern t_config* config_bitmap;
extern t_config* config_bloques;
extern t_config* config_fcbs;
extern t_log* log_filesystem;
extern t_list* fcbs;

bool crear_config(char*, t_config**);
void destruir_config(t_config**);
char* ip_memoria();
char* puerto_memoria();
char* puerto_escucha();
char* superbloque();
char* btmap();
char* bloques();
char* fcb();
int retardo_acceso_bloque();
int block_size();
int block_count();
bool leer_fcbs(char*);
#endif // !_LEER_CONFIG_H_


