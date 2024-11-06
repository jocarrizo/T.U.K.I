#include <kernel.h>

int main(int argc, char** argv){
	log_kernel = log_create("kernel.log", "KERNEL_LOGGER", 1, LOG_LEVEL_DEBUG);

	if(argc < 2){
        log_error(log_kernel, "ERROR - Arhcivo de configuracion no especificado");
		log_destroy(log_kernel);
        return EXIT_FAILURE;
    }
	else if(argc > 2){
		log_error(log_kernel, "ERROR - Demasiados argumentos");
		log_destroy(log_kernel);
        return EXIT_FAILURE;
	}

	crear_config(argv[1]);
	if(!config) {printf("no hay config papaaaaaaaa"); return EXIT_FAILURE;}
	
	conectar_memoria(ip_memoria(), puerto_memoria());
	conectar_cpu(ip_cpu(), puerto_cpu());
	conectar_fs(ip_filesystem(), puerto_filesystem());

	inicializar_threads_respuesta();

	inicializar_tabladearchivos_globales();
	crearListaDeRecursos();
	inicializarColas();
	inicializarSemaforos();
	asignarTareasAHilos();

	socket_kernel = iniciar_servidor(puerto_escucha());	
	conectar_consola(socket_kernel);
	
	esperar_finalizacion();
	
	liberar_conexiones();
	list_destroy_and_destroy_elements(tabla_global_archivos_abiertos, remover_entrada_de_tabla_global);
	destruir_config();
	log_info(log_kernel, "Finaliza kernel");
	log_destroy(log_kernel);
	eliminarListaDeRecursos();
	
	return EXIT_SUCCESS;	
}
