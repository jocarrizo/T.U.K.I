#ifndef _MANEJO_DE_FILESYSTEM_H_
#define _MANEJO_DE_FILESYSTEM_H_
#include <leer_config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <math.h>

extern t_bitarray* bitmap;
extern char* blocks;
extern char* mapeo_bitmap_en_memoria;

void crear_fcb (char*);
int truncar_fcb (char*, int);
void agregar_bloques(t_fcb_filesystem*, int);
void sacar_bloques(t_fcb_filesystem*, int);
void actualizar_fcb(t_fcb_filesystem*);
void sincronizar_bitmap();
void sincronizar_bloques();
t_fcb_filesystem* getFcb(char*);
int cant_bloques_libres();
void* leer_de_archivo(char*, int, int);
void escribir_en_archivo(char*,int,char*,int);

#endif // !_MANEJO_DE_FILESYSTEM_H_