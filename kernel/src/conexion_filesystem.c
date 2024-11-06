#include <conexion_filesystem.h>

void conectar_fs(char* ip, char* puerto){ // Conecta al modulo kernel con el modulo filesystem
	while((socket_fs = crear_conexion(ip, puerto)) <= 0){
		printf("No se pudo establecer conexion con filesystem...\n");
		sleep(5);
	}	
	bool confirmacion;	
	recv(socket_fs, &confirmacion, sizeof(bool), MSG_WAITALL);

	if(confirmacion) printf("Conexion con filesystem exitosa\n");
	else printf("Error, handshake con filesystem fallido\n");	
}

void* recibirMensajefs(int* socket_fs){ // Recibe mensajes que envíe el modulo filesystem
	while(true){
		int msj = recibir_operacion(*socket_fs);
		if(msj == -1 || !system_up){
			log_error(log_kernel, "El filesystem se desconectó");
			system_up = false;
			shutdown(socket_kernel, SHUT_RDWR);
			shutdown(socket_cpu, SHUT_RDWR);
			shutdown(socket_memoria, SHUT_RDWR);
			close(socket_kernel);
			break;
		}
		sem_wait(&buff_fs_sem);
		buff_fs = crear_buffer();
		buff_fs->stream = recibir_buffer(&(buff_fs->size), *socket_fs);//recibir_buffer recibe info del paquete y lo guarda en el string del buffer
		switch(msj){
			case APERTURA:
				sem_post(&apertura_sem);
				break;
			case CREACION:
				sem_post(&creacion_sem);
				break;
			case TRUNCAMIENTO:
				sem_post(&truncamiento_sem);
				break;
			case LECTURA_FS_OK:
				sem_post(&leer_sem);
				break;
			case ESCRITURA_FS_OK:
				sem_post(&escribir_sem);
				break;
			default:
				break;
		}		
	}
	return NULL;
	}	
	
