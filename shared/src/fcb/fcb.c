#include "fcb.h"

void empaquetar_fcb(t_paquete* paquete, t_fcb* fcb){
    int tam_nombre = strlen(fcb->nombre) + 1;
    agregar_a_paquete(paquete, &tam_nombre, sizeof(int));
    agregar_a_paquete(paquete, fcb->nombre, tam_nombre);
    agregar_a_paquete(paquete, &(fcb->tam), sizeof(int));
    agregar_a_paquete(paquete, &(fcb->puntero_directo), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(fcb->puntero_indirecto), sizeof(uint32_t));
}

void enviar_fcb(t_fcb* fcb, int socket_a_enviar){
    t_paquete* paquete = crear_paquete(FCB);
    empaquetar_fcb(paquete, fcb);
    enviar_paquete(paquete, socket_a_enviar);
}

t_fcb* desempaquetar_fcb(void* buff){
    int desplazamiento = 0;
    t_fcb* fcb = (t_fcb*) malloc(sizeof(t_fcb));
    int tam_nombre;
    memcpy(&tam_nombre, buff, sizeof(int));
    desplazamiento += sizeof(int);
    fcb->nombre = (char*) malloc (tam_nombre);
    memcpy(fcb->nombre, buff + desplazamiento, tam_nombre);
    desplazamiento += tam_nombre;
    memcpy(&(fcb->tam), buff + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(fcb->puntero_directo), buff + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(fcb->puntero_indirecto), buff + desplazamiento, sizeof(uint32_t));
    return fcb;
}