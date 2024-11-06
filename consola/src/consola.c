#include <consola.h>

int main(int argc, char **argv){
    log_consola = log_create("consola.log", "CONSOLA_LOGGER", true, LOG_LEVEL_DEBUG);
    
	//argumentos
	FILE *f = son_argumentos_validos(argc, argv);
	if(!f) return EXIT_FAILURE; //no son argumentos validos
	
	//config
	crear_config(argv[1]);

	//conexion
	int socket_kernel = conectar_a_kernel();
	if(!conexion_exitosa(socket_kernel)) return EXIT_FAILURE;

	//empaquetado
	t_paquete *paquete = crear_paquete_instrucciones(f);
	enviar_paquete_instrucciones(paquete, socket_kernel);

	//espera rta de socket
	if(terminar(socket_kernel)) return EXIT_SUCCESS;

	return EXIT_FAILURE;   

}

FILE *son_argumentos_validos(int cantidad_argumentos, char**argv) {

	if (cantidad_argumentos < 3) {
		cantidad_argumentos < 2 ?
				log_error(log_consola, "Archivo de configuración no especificado.\n") :
				log_error(log_consola, "Archivo de pseudocodigo no especificado.\n");
		return NULL;
	}

	FILE *f;
    
	if (!(f = fopen(argv[2], "r"))) {
		log_error(log_consola, "No se encontró el archivo de instrucciones");
		return NULL;
	}

	return f;
}