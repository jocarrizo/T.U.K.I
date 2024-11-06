#include <conexion_kernel.h>

int conectar_a_kernel() {
	if (!config) {
		log_error(log_consola, "No se encontró el archivo de configuracion de Kernel");
		return -1;
	}

	if (!config_has_property(config, "IP_KERNEL") || !config_has_property(config, "PUERTO_KERNEL")) {
		log_error(log_consola, "El archivo de configuración de Kernel es erróneo");
		return -1;
	}

	log_info(log_consola, "Iniciando conexión con Kernel");
	int conexion = crear_conexion(ip_kernel(), puerto_kernel());
	destruir_config();
	return conexion;
}

bool conexion_exitosa(int socket) {
	if (socket == -1) {
		log_error(log_consola, "No se ha podido establecer comunicación con el Kernel");
		return false;
	}
	else {
		log_info(log_consola, "Conexión exitosa!");
		return true;
	}
}

void enviar_paquete_instrucciones(t_paquete *paquete, int socket_kernel) {
	log_info(log_consola, "Enviando paquete");
	enviar_paquete(paquete, socket_kernel);
	log_info(log_consola, "Paquete enviado");
}

bool terminar(int socket_kernel) {
	op_code mensaje;
	recv(socket_kernel, &mensaje, sizeof(op_code), MSG_WAITALL);

	bool termino_exitoso = mensaje == TERMINAR_CONSOLA;

	if (termino_exitoso) 
		log_info(log_consola, "Terminando consola");	
	
	else
		log_error(log_consola, "Operación desconocida");

	log_destroy(log_consola);
	liberar_conexion(socket_kernel);
	return termino_exitoso;
}