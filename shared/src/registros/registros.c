#include "registros.h"

int get_tam_reg(char* registro_txt){
    if(!strcmp("AX", registro_txt))
        return 4;
    if(!strcmp("BX", registro_txt))
        return 4;
    if(!strcmp("CX", registro_txt))
        return 4;
    if(!strcmp("DX", registro_txt))
        return 4;
    if(!strcmp("EAX", registro_txt))
        return 8;
    if(!strcmp("EBX", registro_txt))
        return 8;
    if(!strcmp("ECX", registro_txt))
        return 8;
    if(!strcmp("EDX", registro_txt))
        return 8;
    if(!strcmp("RAX", registro_txt))
        return 16;
    if(!strcmp("RBX", registro_txt))
        return 16;
    if(!strcmp("RCX", registro_txt))
        return 16;
    if(!strcmp("RDX", registro_txt))
        return 16;

    return -1;
}

void asignar_registro(char* registro_txt, char* val, t_registros* registros){
    if(!strcmp("AX", registro_txt))
        memcpy((registros->AX), val, 4);
    if(!strcmp("BX", registro_txt))
        memcpy((registros->BX), val, 4);
    if(!strcmp("CX", registro_txt))
        memcpy((registros->CX), val, 4);
    if(!strcmp("DX", registro_txt))
        memcpy((registros->DX), val, 4);
    if(!strcmp("EAX", registro_txt))
        memcpy((registros->EAX), val, 8);
    if(!strcmp("EBX", registro_txt))
        memcpy((registros->EBX), val, 8);
    if(!strcmp("ECX", registro_txt))
        memcpy((registros->ECX), val, 8);
    if(!strcmp("EDX", registro_txt))
        memcpy((registros->EDX), val, 8);
    if(!strcmp("RAX", registro_txt))
        memcpy((registros->RAX), val, 16);
    if(!strcmp("RBX", registro_txt))
        memcpy((registros->RBX), val, 16);
    if(!strcmp("RCX", registro_txt))
        memcpy((registros->RCX), val, 16);
    if(!strcmp("RDX", registro_txt))
        memcpy((registros->RDX), val, 16);
}

char* get_valor_reg(char* registro_txt, t_registros* registros){
    int tam = get_tam_reg(registro_txt) + 1;
    char* respuesta = (char*) malloc(tam);
    if(!strcmp("AX", registro_txt))
        memcpy(respuesta, (registros->AX), tam - 1);
    if(!strcmp("BX", registro_txt))
        memcpy(respuesta, (registros->BX), tam - 1);
    if(!strcmp("CX", registro_txt))
        memcpy(respuesta, (registros->CX), tam - 1);
    if(!strcmp("DX", registro_txt))
        memcpy(respuesta, (registros->DX), tam - 1);
    if(!strcmp("EAX", registro_txt))
        memcpy(respuesta, (registros->EAX), tam - 1);
    if(!strcmp("EBX", registro_txt))
        memcpy(respuesta, (registros->EBX), tam - 1);
    if(!strcmp("ECX", registro_txt))
        memcpy(respuesta, (registros->ECX), tam - 1);
    if(!strcmp("EDX", registro_txt))
        memcpy(respuesta, (registros->EDX), tam - 1);
    if(!strcmp("RAX", registro_txt))
        memcpy(respuesta, (registros->RAX), tam - 1);
    if(!strcmp("RBX", registro_txt))
        memcpy(respuesta, (registros->RBX), tam - 1);
    if(!strcmp("RCX", registro_txt))
        memcpy(respuesta, (registros->RCX), tam - 1);
    if(!strcmp("RDX", registro_txt))
        memcpy(respuesta, (registros->RDX), tam - 1);
    respuesta[tam - 1] = '\0';
    return respuesta;
}