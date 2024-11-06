#include <conexion_memoria.h>

void conectar_memoria(char* ip, char* puerto){ // Conecta al modulo filesystem con el modulo memoria
    
	while((socket_memoria = crear_conexion(ip, puerto)) <= 0){
		log_info(log_filesystem, "No se pudo establecer conexion con memoria...");
		sleep(5);
	}
	
	bool confirmacion;
	send(socket_memoria, &id_fs, sizeof(int), 0);
    
	recv(socket_memoria, &confirmacion, sizeof(bool), MSG_WAITALL);

	if(confirmacion) log_info(log_filesystem, "Conexion con memoria exitosa");
	else log_error(log_filesystem, "Error, handshake con memoria fallido");
}
