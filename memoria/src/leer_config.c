#include <leer_config.h>

void crear_config(char* config_path){
    config = config_create(config_path);
}

void destruir_config(){
    config_destroy(config);
}

char* puerto_escucha(){
    return config_get_string_value(config, "PUERTO_ESCUCHA");
}
int tam_memoria(){
    return config_get_int_value(config, "TAM_MEMORIA");
}

int tam_segmento_0(){
    return config_get_int_value(config, "TAM_SEGMENTO_0");
}

int cant_segmentos(){
    return config_get_int_value(config, "CANT_SEGMENTOS");
}
int retardo_memoria(){
    return config_get_int_value(config, "RETARDO_MEMORIA");
}
int retardo_compactacion(){
    return config_get_int_value(config, "RETARDO_COMPACTACION");
}
char* algoritmo_asignacion(){
    return config_get_string_value(config, "ALGORITMO_ASIGNACION");
}