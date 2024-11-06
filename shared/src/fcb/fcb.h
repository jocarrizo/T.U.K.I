#ifndef _FCB_H_
#define _FCB_H_
#include <stdint.h>
#include <sockets/sockets.h>

typedef struct {
    char* nombre;
    int tam;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;
}t_fcb;

void empaquetar_fcb(t_paquete*, t_fcb*);
void enviar_fcb(t_fcb*, int);
t_fcb* desempaquetar_fcb(void*);

#endif // !_FCB_H_