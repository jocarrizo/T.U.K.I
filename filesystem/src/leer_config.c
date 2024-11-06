#include <leer_config.h>


bool crear_config(char* config_path, t_config** conf){
    *conf = config_create(config_path);
    return *conf != NULL;
}

void destruir_config(t_config** conf){
    config_destroy(*conf);
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

char* superbloque(){
    return config_get_string_value(config, "PATH_SUPERBLOQUE");
}

char* btmap(){
    return config_get_string_value(config, "PATH_BITMAP");
}

char* bloques(){
    return config_get_string_value(config, "PATH_BLOQUES");
}

char* fcb(){
    return config_get_string_value(config, "PATH_FCB");
}

int retardo_acceso_bloque(){
    return config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");
}

int block_size(){
    return config_get_int_value(config_superbloque, "BLOCK_SIZE");
}

int block_count(){
    return config_get_int_value(config_superbloque, "BLOCK_COUNT");
}

bool leer_fcbs(char* path_dir_fcbs){
    t_config* fcb_config;
    t_fcb* fcb;
    t_fcb_filesystem* fcb_;
    DIR *dir;
    struct dirent *entrada;

    // Abrir el directorio
    dir = opendir(path_dir_fcbs);
    if (dir == NULL)      
        return false;

    fcbs = list_create();
    
    // Leer cada entrada en el directorio
    while ((entrada = readdir(dir)) != NULL) {
        // Ignorar las entradas "." y ".."
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        // Construir la ruta completa del archivo
        int tam_ruta = strlen(path_dir_fcbs) + strlen(entrada->d_name) + 2;
        char* ruta_archivo = (char*) malloc(tam_ruta);
        snprintf(ruta_archivo, tam_ruta, "%s/%s", path_dir_fcbs, entrada->d_name);
        // Leer el archivo
        
        fcb_config = config_create(ruta_archivo);
        if (fcb_config == NULL) {
            log_info(log_filesystem, "No se pudo abrir el archivo: %s", ruta_archivo);
            continue;
        }
        fcb = (t_fcb*) malloc (sizeof(t_fcb));
        fcb_ = (t_fcb_filesystem*) malloc (sizeof(t_fcb_filesystem));
        fcb->nombre = config_get_string_value(fcb_config, "NOMBRE_ARCHIVO");
        fcb->tam = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
        fcb->puntero_directo = config_get_int_value(fcb_config, "PUNTERO_DIRECTO"); 
        fcb->puntero_indirecto = config_get_int_value(fcb_config, "PUNTERO_INDIRECTO");
        fcb_->fcb = fcb;
        fcb_->fcb_config = fcb_config;
        free(ruta_archivo);
        list_add(fcbs, fcb_);
    }

    closedir(dir);
    return true;     
}
