#include <leer_config.h>

void crear_config(char* config_path){
    config = config_create(config_path);
}

void destruir_config(){
    config_destroy(config);
}

char* ip_kernel(){
    return config_get_string_value(config, "IP_KERNEL");
}
char* puerto_kernel(){    
    return config_get_string_value(config, "PUERTO_KERNEL");
}