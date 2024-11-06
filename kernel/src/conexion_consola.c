#include <conexion_consola.h>

void conectar_consola(int servidor){ 
	pthread_t thread;

	while(system_up){
		sem_wait(&sock_libre);
		int consola = esperar_cliente(servidor);
		if(consola == -1) continue;
		log_info(log_kernel, "Se conectó una consola con id: %d\n", consola);
		
		pthread_create(&thread, NULL, (void*) proceso_new, &consola);
		pthread_detach(thread);
	}
}

void recibir_paquete_consola(void *buffer, int size, t_pcb *pcb) {
	int desplazamiento = 0;

	pcb->contexto_ejecucion->instrucciones = list_create();

	while(desplazamiento < size)
		list_add(pcb->contexto_ejecucion->instrucciones, desempaquetar_instruccion(buffer, &desplazamiento));	
}

t_pcb* generar_pcb(int socket_cliente){
	int size;
	void *buffer = recibir_buffer(&size, socket_cliente);

	t_pcb* pcb = (t_pcb *) malloc(sizeof(t_pcb));
	pcb->fd = socket_cliente;
	pcb->contexto_ejecucion = (t_contexto_ejecucion*) malloc(sizeof(t_contexto_ejecucion));

	pthread_mutex_lock(&lock);
	pcb->contexto_ejecucion->id = cant_procesos++;
 	pthread_mutex_unlock(&lock);

	recibir_paquete_consola(buffer, size, pcb); 
	pcb->contexto_ejecucion->program_counter = 0;
	pcb->est_rafaga = estimacion_inicial();
	pcb->contexto_ejecucion->tiempo_real_ej = 0;
	pcb->tabla_de_recursos = (int*)calloc(list_size(recursos), sizeof(int));
	
	free(buffer);
	return pcb;
}

void* proceso_new(int* socket_consola){
	int sock_consola = *socket_consola;
	sem_post(&sock_libre);

	if(!(recibir_operacion(sock_consola) == INSTRUCCIONES_CONSOLA)){
		log_error(log_kernel, "Operacion no reconocida");
		int terminar_error = 10;
		send(sock_consola, &terminar_error, sizeof(int), 0);				
		return NULL;
	}

	t_pcb* pcb = generar_pcb(sock_consola);

	agregar_new(pcb); 

	return NULL;	 
}
