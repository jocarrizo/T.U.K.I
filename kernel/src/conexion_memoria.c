#include <conexion_memoria.h>

void conectar_memoria(char* ip, char* puerto){ // Conecta al modulo kernel con el modulo memoria
	
	while((socket_memoria = crear_conexion(ip, puerto)) <= 0){
		log_info(log_kernel, "No se pudo establecer conexion con memoria...");
		sleep(5);
	}	
	bool confirmacion;
	send(socket_memoria, &id_kernel, sizeof(int), 0);
	recv(socket_memoria, &confirmacion, sizeof(bool), MSG_WAITALL);

	if(confirmacion) log_info(log_kernel, "Conexion con memoria exitosa");
	else log_error(log_kernel, "Error, handshake con memoria fallido");
}

void* recibirMensajeMemoria(int* socket_memoria){ 
	// Recibe mensajes que envíe el modulo memoria
	//TODO: Recibir mensajes de memoria
	while(true){
		int msj = recibir_operacion(*socket_memoria);
		if(msj == -1 || !system_up){
			log_error(log_kernel, "La memoria se desconectó");
			system_up = false;
			shutdown(socket_kernel, SHUT_RDWR);
			shutdown(socket_fs, SHUT_RDWR);
			shutdown(socket_cpu, SHUT_RDWR);
			close(socket_kernel);
			break;
		}
		sem_wait(&buff_memoria_sem);
		buff_memoria = crear_buffer();
		buff_memoria->stream = recibir_buffer(&(buff_memoria->size), *socket_memoria);
		
		switch (msj)
		{
		case TABLA_SEGMENTOS:
			sem_post(&recepcion_tabla);
			break;
		case BASE_SEGMENTO:
			sem_post(&recepcion_base);
			break;
		case TABLA_SEGMENTOS_ACTUALIZADA:
			sem_post(&tabla_actualizada_sem);
			break;

		case TABLAS_DE_SEGMENTOS:
			sem_post(&tablas_actualizadas_sem);
			break;
		case MENSAJE:
			if(strcmp("ELIMINACION PROC OK", buff_memoria->stream) == 0)
				sem_post(&eliminacion_proceso_sem);

			liberar_buffer(buff_memoria);
			sem_post(&buff_memoria_sem);	
		default:
			break;
		}
					
	}	
	return NULL;
}
