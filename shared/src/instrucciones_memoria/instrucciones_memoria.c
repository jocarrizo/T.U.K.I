#include "instrucciones_memoria.h"

char* leer_de_memoria(int dir_fisica, int cant_bytes, int pid, int socket_memoria){
    t_paquete* paquete = crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = LEER;
    agregar_a_paquete(paquete, &codigo, sizeof(id_instruccion_memoria));
    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, &dir_fisica, sizeof(int));
    agregar_a_paquete(paquete, &cant_bytes, sizeof(int));
    enviar_paquete(paquete, socket_memoria);    
    recibir_operacion(socket_memoria);
    
    char* msj = (char*)recibir_mensaje(socket_memoria);
    return msj;    
}

void escribir_en_memoria(int dir_fisica, void* valor, int cant_bytes, int pid, int socket_memoria){
    t_paquete* paquete = crear_paquete(INSTRUCCIONES_MEMORIA);
    id_instruccion_memoria codigo = ESCRIBIR;
    agregar_a_paquete(paquete, &codigo, sizeof(id_instruccion_memoria));
    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, &dir_fisica, sizeof(int));
    agregar_a_paquete(paquete, &cant_bytes, sizeof(int));
    agregar_a_paquete(paquete, valor, cant_bytes);
    enviar_paquete(paquete, socket_memoria);
    recibir_operacion(socket_memoria);
    char* msj = (char*)recibir_mensaje(socket_memoria);
    free(msj);
}
