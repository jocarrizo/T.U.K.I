#include "segmentos.h"

void liberar_segmento(t_segmento* seg){
	free(seg);
}

void empaquetar_tabla_segs(t_paquete* paquete, t_tabla_de_segmentos* tabla_de_segmentos){
	char hay_segmento = 'S';
	char no_hay_segmento = 'N';
	agregar_a_paquete(paquete, &(tabla_de_segmentos->pid), sizeof(int));
	agregar_a_paquete(paquete, &(tabla_de_segmentos->tam), sizeof(int));
	for(int i = 0; i < tabla_de_segmentos->tam; i++){
		if(tabla_de_segmentos->segmentos[i] == 	NULL)
			agregar_a_paquete(paquete, &no_hay_segmento, sizeof(char));
		else{
			agregar_a_paquete(paquete, &hay_segmento, sizeof(char));
			agregar_a_paquete(paquete, tabla_de_segmentos->segmentos[i], sizeof(t_segmento));
		}		
	}	
}

t_segmento* desempaquetar_segmento(void* buff, int* desplazamiento){
	t_segmento* seg = (t_segmento*) calloc(1, sizeof(t_segmento));
	memcpy(seg, buff + *desplazamiento, sizeof(t_segmento));
	*desplazamiento += sizeof(t_segmento);	
	return seg;
}



t_tabla_de_segmentos* desempaquetar_tabla_segs(void* buff, int* desplazamiento){	
	char codigo;	
	t_tabla_de_segmentos* tabla = (t_tabla_de_segmentos*) malloc(sizeof(t_tabla_de_segmentos));
	memcpy(&(tabla->pid), buff + *desplazamiento, sizeof(int));
	*desplazamiento += sizeof(int);
	memcpy(&(tabla->tam), buff + *desplazamiento, sizeof(int));
	*desplazamiento += sizeof(int);
	tabla->segmentos = (t_segmento**) calloc(tabla->tam, sizeof(t_segmento*));
	for(int i = 0; i < tabla->tam; i++){
		memcpy(&codigo, buff + *desplazamiento, sizeof(char));
		*desplazamiento += sizeof(char);
		if(codigo == 'N')
			continue;
		tabla->segmentos[i] = desempaquetar_segmento(buff, desplazamiento);
	}
	return tabla;
}

void liberar_tabla_segs_completa(t_tabla_de_segmentos* tabla){
	for(int i = 0; i < tabla->tam; i++){
		if(tabla->segmentos[i] != NULL)
			liberar_segmento(tabla->segmentos[i]);
	}
	free(tabla->segmentos);
	free(tabla);
}

void liberar_tabla_segs(t_tabla_de_segmentos* tabla){
	for(int i = 1; i < tabla->tam; i++){
		if(tabla->segmentos[i] != NULL)
			liberar_segmento(tabla->segmentos[i]);
	}
	free(tabla->segmentos);
	free(tabla);
}

void enviar_tabla_segs(t_tabla_de_segmentos* tabla, int socket_a_enviar, int codigo){
	t_paquete* paquete = crear_paquete(codigo);
	empaquetar_tabla_segs(paquete, tabla);
	enviar_paquete(paquete, socket_a_enviar);
}

t_tabla_de_segmentos* crear_tabla_segs(int tam, int pid){
	t_tabla_de_segmentos* tabla = (t_tabla_de_segmentos*) malloc(sizeof(t_tabla_de_segmentos));
	tabla->tam = tam;
	tabla->pid = pid;
	tabla->segmentos = (t_segmento**) calloc (tam, sizeof(t_segmento*));	
	return tabla;
}

