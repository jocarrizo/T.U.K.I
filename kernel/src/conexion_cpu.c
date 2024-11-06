#include <conexion_cpu.h>


void conectar_cpu(char* ip, char* puerto){ // Conecta al modulo kernel con el modulo cpu
	while((socket_cpu = crear_conexion(ip, puerto)) <= 0){
		printf("No se pudo establecer conexion con cpu...\n");
		sleep(5);
	}
	bool confirmacion;
	recv(socket_cpu, &confirmacion, sizeof(bool), MSG_WAITALL);

	if(confirmacion) printf("Conexion con cpu exitosa\n");
	else printf("Error, handshake con cpu fallido\n");	
}

void* recibirMensajecpu(int* socket_cpu){ // Recibe mensajes que envíe el modulo cpu
	
	while(true){
		int cod_op_rta = recibir_operacion(*socket_cpu);
		if(cod_op_rta == -1 || !system_up){
			log_error(log_kernel, "El cpu se desconectó");
			system_up = false;
			shutdown(socket_kernel, SHUT_RDWR);
			shutdown(socket_fs, SHUT_RDWR);
			shutdown(socket_memoria, SHUT_RDWR);
			close(socket_kernel);
			break;
		}
		t_buffer* buffer_rta = crear_buffer();

		buffer_rta->stream = recibir_buffer(&(buffer_rta->size), *socket_cpu);
		liberar_contexto(pcb_exec->contexto_ejecucion);
		pcb_exec->contexto_ejecucion = desempaquetar_contexto(buffer_rta->stream);
		if(cod_op_rta == F_WRITE_ || cod_op_rta == F_READ_)
			memcpy(&dir_fisica, buffer_rta->stream + buffer_rta->size - sizeof(int), sizeof(int));
		liberar_buffer(buffer_rta);
		realizar_op(cod_op_rta);
	}

return NULL;
}