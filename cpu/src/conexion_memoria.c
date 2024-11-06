#include <conexion_memoria.h>

void conectar_memoria(char* ip, char* puerto){ // Conecta al modulo cpu con el modulo memoria 
	while((socket_memoria = crear_conexion(ip, puerto)) <= 0){
		log_warning(log_cpu, "No se pudo establecer conexion con memoria...\n");
		sleep(5);
	}
	
	bool confirmacion;
	send(socket_memoria, &id_cpu, sizeof(int), 0);
	recv(socket_memoria, &confirmacion, sizeof(bool), MSG_WAITALL);

	if(confirmacion) log_info(log_cpu, "Conexion con memoria exitosa\n");
	else log_error(log_cpu, "Error, handshake con memoria fallido\n");
}
/*
void* recibirMensajeMemoria(int* socket_memoria){ // Recibe mensajes que envíe el modulo memoria
	while(true){
		log_info(log_cpu, "%s", (char*)recibir_mensaje(*socket_memoria));	
	}
    return NULL;
}
*/