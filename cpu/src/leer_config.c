#include <leer_config.h>

void crear_config(char* config_path){
    config = config_create(config_path);
}

void destruir_config(){
    config_destroy(config);
}

char* ip_memoria(){
    return config_get_string_value(config, "IP_MEMORIA");
}

char* puerto_memoria(){
    return config_get_string_value(config, "PUERTO_MEMORIA");
}

char* puerto_escucha(){
    return config_get_string_value(config, "PUERTO_ESCUCHA");
}

int retardo_instruccion(){
    return config_get_int_value(config, "RETARDO_INSTRUCCION");
}

int tam_max_segmento(){
    return config_get_int_value(config, "TAM_MAX_SEGMENTO");
}