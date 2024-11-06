#include <cpu.h>

int main(int argc, char** argv){
    log_cpu = log_create("cpu.log", "CPU_LOGGER", 1, LOG_LEVEL_DEBUG);

	if(argc < 2){
        log_error(log_cpu, "ERROR - Arhcivo de configuracion no especificado");
		log_destroy(log_cpu);
        return EXIT_FAILURE;
    }
	else if(argc > 2){
		log_error(log_cpu, "ERROR - Demasiados argumentos");
		log_destroy(log_cpu);
        return EXIT_FAILURE;
	}

	crear_config(argv[1]);
	if(!config) {printf("no hay config papaaaaaaaa"); return EXIT_FAILURE;}

    inicializar_conexiones();    
    
    pthread_join(conexion_memoria, NULL);

    liberar_conexiones();

    destruir_config();
    log_info(log_cpu, "Finaliza cpu");
    log_destroy(log_cpu);

    return EXIT_SUCCESS;    
}
