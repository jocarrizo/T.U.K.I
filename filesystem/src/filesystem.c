#include <filesystem.h>

int main(int argc, char** argv){
    log_filesystem = log_create("filesystem.log", "FILESYSTEM_LOGGER", true, LOG_LEVEL_DEBUG);
    if(argc < 2){
        log_error(log_filesystem, "ERROR - Arhcivo de configuracion no especificado");
		log_destroy(log_filesystem);
        return EXIT_FAILURE;
    }

	else if(argc > 2){
		log_error(log_filesystem, "ERROR - Demasiados argumentos");
		log_destroy(log_filesystem);
        return EXIT_FAILURE;
	}

	
	if(!crear_config(argv[1], &config)) {log_error(log_filesystem, "No se pudo abrir el archivo de configuración"); log_destroy(log_filesystem); return EXIT_FAILURE;}
    if(!crear_config(superbloque(), &config_superbloque)) {log_error(log_filesystem, "No se pudo abrir el archivo de superbloques"); log_destroy(log_filesystem); return EXIT_FAILURE;}
        
    conectar_memoria(ip_memoria(), puerto_memoria());

    int fdbtmp = open(btmap(), O_RDWR, S_IRUSR | S_IWUSR);
    struct stat fsbtmp;
    
    if(fstat(fdbtmp, &fsbtmp) == -1) {
        log_error(log_filesystem, "No se pudo abrir el archivo de bitmap");
        log_destroy(log_filesystem);
        destruir_config(&config);
        destruir_config(&config_superbloque);
        liberar_conexion(socket_memoria);
        return EXIT_FAILURE;
    }

    int fdblqs = open(bloques(), O_RDWR, S_IRUSR | S_IWUSR);
    struct stat fsblqs;
    
    if(fstat(fdblqs, &fsblqs) == -1) {
        log_error(log_filesystem, "No se pudo abrir el archivo de bloques");
        log_destroy(log_filesystem);
        destruir_config(&config);
        destruir_config(&config_superbloque);
        liberar_conexion(socket_memoria);
        return EXIT_FAILURE;
    }

    if(!leer_fcbs(fcb())){
        log_error(log_filesystem, "No se pudo leer el directorio de fcbs");
        log_destroy(log_filesystem);
        destruir_config(&config);
        destruir_config(&config_superbloque);
        liberar_conexion(socket_memoria);
        return EXIT_FAILURE;
    }
    
    mapeo_bitmap_en_memoria = mmap(NULL, ceil((float)block_count() / 8), PROT_READ | PROT_WRITE, MAP_SHARED, fdbtmp, 0);
    bitmap = bitarray_create_with_mode(mapeo_bitmap_en_memoria, ceil((float)block_count() / 8), MSB_FIRST);
    
    blocks = mmap(NULL, block_count() * block_size(), PROT_READ | PROT_WRITE, MAP_SHARED, fdblqs, 0);

    socket_fs = iniciar_servidor(puerto_escucha());
    conectar_kernel(socket_fs);

    liberar_conexiones();

    destruir_config(&config_superbloque);
    destruir_config(&config);
    log_info(log_filesystem, "Finaliza filesystem");
    log_destroy(log_filesystem);

    return EXIT_SUCCESS;    
}

