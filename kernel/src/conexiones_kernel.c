#include <conexiones_kernel.h>

void inicializar_threads_respuesta(){ // Crea los hilos de comunicacion con los otros modulos para recibir mensajes
	pthread_create(&conexion_memoria, NULL, (void*)recibirMensajeMemoria, &socket_memoria);
	pthread_create(&conexion_cpu	, NULL, (void*)recibirMensajecpu	, &socket_cpu);
	pthread_create(&conexion_fs		, NULL, (void*)recibirMensajefs		, &socket_fs);
}

void esperar_finalizacion(){ // Espera la finalizacion de los hilos de comunicacion con los otros modulos
	pthread_join(conexion_cpu, NULL);
	pthread_join(conexion_memoria, NULL);
	pthread_join(conexion_fs, NULL);
}

void liberar_conexiones(){ // Libera las conexiones con los otros modulos
	liberar_conexion(socket_cpu);
	liberar_conexion(socket_memoria);
	liberar_conexion(socket_fs);
}