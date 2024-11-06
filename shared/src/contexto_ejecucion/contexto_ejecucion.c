#include "contexto_ejecucion.h"

/*
void imprimir_pcb(t_pcb *pcb) {
	printf("\n\nConsola: %d\n", pcb->fd);
	imprimir_contexto(pcb->contexto_ejecucion);
	printf("Llegada a Ready: %f\nEstimado de Ráfaga Siguiente: %f\n\n", pcb->llegada_a_ready, pcb->est_rafaga);	
}
*/
void imprimir_registros(t_registros* regs){
	printf("AX: ");
	for(int i = 0; i < 4; i++)
		printf("%c", regs->AX[i]);
	printf("\nBX: ");
	for(int i = 0; i < 4; i++)
		printf("%c", regs->	BX[i]);
	printf("\nCX: ");
	for(int i = 0; i < 4; i++)
		printf("%c", regs->	CX[i]);
	printf("\nDX: ");
	for(int i = 0; i < 4; i++)
		printf("%c", regs->	DX[i]);
	printf("\nEAX: ");
	for(int i = 0; i < 8; i++)
		printf("%c", regs->	EAX[i]);
	printf("\nEBX: ");
	for(int i = 0; i < 8; i++)
		printf("%c", regs->	EBX[i]);
	printf("\nECX: ");
	for(int i = 0; i < 8; i++)
		printf("%c", regs->	ECX[i]);
	printf("\nEDX: ");
	for(int i = 0; i < 8; i++)
		printf("%c", regs->	EDX[i]);
	printf("\nRAX: ");
	for(int i = 0; i < 16; i++)
		printf("%c", regs->	RAX[i]);
	printf("\nRBX: ");
	for(int i = 0; i < 16; i++)
		printf("%c", regs->	RBX[i]);
	printf("\nRCX: ");
	for(int i = 0; i < 16; i++)
		printf("%c", regs->	RCX[i]);
	printf("\nRDX: ");
	for(int i = 0; i < 16; i++)
		printf("%c", regs->	RDX[i]);
	printf("\n");
}

void imprimir_contexto(t_contexto_ejecucion* contexto){
	printf("\n\nPID: %d\nCantidad de instrucciones: %d\nInstrucciones:\n\n", contexto->id, list_size(contexto->instrucciones));
	list_iterate(contexto->instrucciones, (void *) imprimir_instruccion);
	printf("\n\nProgram Counter: %d\nTiempo Real de Ejecucion: %.0lf\nRegistros:\n\n", contexto->program_counter, contexto->tiempo_real_ej);
	imprimir_registros(&(contexto->registros));		
}


void liberar_contexto(t_contexto_ejecucion* contexto){
	list_destroy_and_destroy_elements(contexto->instrucciones, (void*) liberar_instruccion);
	liberar_tabla_segs_completa(contexto->tabla_segmentos);
	free(contexto);
}

t_contexto_ejecucion* desempaquetar_contexto(void* buffer){
	int desplazamiento = 0;
	int cant_instr = 0;	
	t_contexto_ejecucion* contexto = (t_contexto_ejecucion*) malloc(sizeof(t_contexto_ejecucion));
	assert(contexto != NULL);
	contexto->instrucciones = list_create();	

	memcpy(&(contexto->id), buffer + desplazamiento, sizeof(uint));
	desplazamiento += sizeof(uint);
	memcpy(&(contexto->program_counter), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(contexto->registros), buffer + desplazamiento, sizeof(t_registros));
	desplazamiento += sizeof(t_registros);
	memcpy(&(contexto->tiempo_real_ej), buffer + desplazamiento, sizeof(double));
	desplazamiento += sizeof(double);
	memcpy(&cant_instr, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	
	for (int i = 0; i < cant_instr; i++)
		list_add(contexto->instrucciones, desempaquetar_instruccion(buffer, &desplazamiento));

	contexto->tabla_segmentos = desempaquetar_tabla_segs(buffer, &desplazamiento);
	return contexto;
}

/*
t_pcb *desempaquetar_pcb(void *buffer) {
	int desplazamiento = 0;
	t_pcb *pcb = (t_pcb *) malloc(sizeof(t_pcb));
	assert(pcb != NULL);	

	memcpy(&(pcb->fd), buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&(pcb->tabla_segmentos), buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&(pcb->est_rafaga), buffer + desplazamiento, sizeof(double));
	desplazamiento += sizeof(double);
	memcpy(&(pcb->llegada_a_ready), buffer + desplazamiento, sizeof(double));
	desplazamiento += sizeof(double);
	memcpy(&(pcb->tabla_de_archivos), buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	
	pcb->contexto_ejecucion = desempaquetar_contexto(buffer + desplazamiento);

	return pcb;
}
*/



void empaquetar_contexto(t_paquete* paq, t_contexto_ejecucion* contexto){
	assert(contexto != NULL);
	assert(paq != NULL);
	int cant_instr = list_size(contexto->instrucciones);
	agregar_a_paquete(paq, &(contexto->id), sizeof(uint));
	agregar_a_paquete(paq, &(contexto->program_counter), sizeof(uint32_t));
	agregar_a_paquete(paq, &(contexto->registros), sizeof(t_registros));
	agregar_a_paquete(paq, &(contexto->tiempo_real_ej), sizeof(double));
	agregar_a_paquete(paq, &cant_instr, sizeof(int));
	t_instruccion* siguiente;
	t_list_iterator *iterador_instrucciones = list_iterator_create(contexto->instrucciones);
	while(list_iterator_has_next(iterador_instrucciones)){
		siguiente = (t_instruccion*) list_iterator_next(iterador_instrucciones);
		assert(siguiente != NULL);
		empaquetar_instruccion(paq, siguiente);
	}
	list_iterator_destroy(iterador_instrucciones);
	empaquetar_tabla_segs(paq, contexto->tabla_segmentos);
}
/*
void empaquetar_pcb(t_paquete *paquete_pcb, t_pcb *pcb) {
	assert(pcb != NULL);
	assert(paquete_pcb != NULL);
	
	agregar_a_paquete(paquete_pcb, &(pcb->fd), sizeof(int));
	agregar_a_paquete(paquete_pcb, &(pcb->tabla_segmentos), sizeof(int));	
	agregar_a_paquete(paquete_pcb, &(pcb->est_rafaga), sizeof(double));
	agregar_a_paquete(paquete_pcb, &(pcb->llegada_a_ready), sizeof(double));
	agregar_a_paquete(paquete_pcb, &(pcb->tabla_de_archivos), sizeof(int));
	empaquetar_contexto(paquete_pcb, pcb->contexto_ejecucion);
	//liberar_pcb(pcb);
}
*/

void enviar_contexto(t_contexto_ejecucion* contexto, int socket_a_enviar, int Codigo){
	t_paquete *paquete_contexto = crear_paquete(Codigo);
	empaquetar_contexto(paquete_contexto, contexto);
	enviar_paquete(paquete_contexto, socket_a_enviar);
}



/*
void enviar_pcb(t_pcb *pcb, int socket_a_enviar, int Codigo) {
	t_paquete *paquete_pcb = crear_paquete(Codigo);
	empaquetar_pcb(paquete_pcb, pcb);
	enviar_paquete(paquete_pcb, socket_a_enviar);
}
*/