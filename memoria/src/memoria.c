#include <memoria.h>
//VER TEMA CONFIG 

int main(int argc, char** argv){
    log_memoria = log_create("memoria.log", "MEMORIA_LOGGER", true, LOG_LEVEL_DEBUG);
    if(argc < 2){
        log_error(log_memoria, "ERROR - Arhcivo de configuracion no especificado");
		log_destroy(log_memoria);
        return EXIT_FAILURE;
    }
	else if(argc > 2){
		log_error(log_memoria, "ERROR - Demasiados argumentos");
		log_destroy(log_memoria);
        return EXIT_FAILURE;
	}
    
    crear_config(argv[1]);
    if(!config) {log_error(log_memoria,"No se puede abrir el archivo de configuración"); log_destroy(log_memoria); return EXIT_FAILURE;}
    //ACA HAY QUE INICIALIZAR LA MEMORIA
    inicializarSemaforo();
    inicializar_memoria();    
    inicializarConexiones();
    destruir_config();
    log_info(log_memoria, "Finaliza memoria");      
    log_destroy(log_memoria);    
    return 0;
}

