#include <instrucciones/instrucciones.h>


uint8_t get_identificador(char *identificador_leido) {
	uint8_t identificador = NO_RECONOCIDO;
	if (!strcmp("F_READ", identificador_leido))
		identificador = F_READ;
	if (!strcmp("F_WRITE", identificador_leido))
		identificador = F_WRITE;
	if (!strcmp("SET", identificador_leido))
		identificador = SET;	
	if (!strcmp("MOV_IN", identificador_leido))
		identificador = MOV_IN;
	if (!strcmp("MOV_OUT", identificador_leido))
		identificador = MOV_OUT;
	if (!strcmp("F_TRUNCATE", identificador_leido))
		identificador = F_TRUNCATE;
	if (!strcmp("F_SEEK", identificador_leido))
		identificador = F_SEEK;
	if (!strcmp("CREATE_SEGMENT", identificador_leido))
		identificador = CREATE_SEGMENT;
	if (!strcmp("I/O", identificador_leido))
		identificador = IO;
	if (!strcmp("WAIT", identificador_leido))
		identificador = WAIT;
	if (!strcmp("SIGNAL", identificador_leido))
		identificador = SIGNAL;
	if (!strcmp("F_OPEN", identificador_leido))
		identificador = F_OPEN;
	if (!strcmp("F_CLOSE", identificador_leido))
		identificador = F_CLOSE;
	if (!strcmp("DELETE_SEGMENT", identificador_leido))
		identificador = DELETE_SEGMENT;	
	if (!strcmp("EXIT\n", identificador_leido))
		identificador = EXIT;
	if (!strcmp("EXIT", identificador_leido))
		identificador = EXIT;
	if (!strcmp("YIELD\n", identificador_leido))
		identificador = YIELD;
	if (!strcmp("YIELD", identificador_leido))
		identificador = YIELD;
	return identificador;
}


uint8_t get_cant_parametros(uint8_t identificador) {
	uint8_t cant_parametros = 0;
	switch (identificador) {
	case EXIT: case YIELD:
		break;

	case IO: case WAIT: case SIGNAL: case F_OPEN: case F_CLOSE: case DELETE_SEGMENT: 
		cant_parametros = 1;
		break;

	case SET: case MOV_IN: case MOV_OUT: case F_TRUNCATE: case F_SEEK: case CREATE_SEGMENT:
		cant_parametros = 2;
		break;
	
    case F_READ: case F_WRITE:
        cant_parametros = 3;        
    }
        
    
	return cant_parametros;
}

t_instruccion * crear_instruccion(uint8_t identificador, uint8_t cant_parametros, char** parametros) {
	t_instruccion *instruccion = (t_instruccion *) malloc(sizeof(t_instruccion));
	assert(instruccion != NULL);
	instruccion->identificador = identificador;
	instruccion->cant_parametros = cant_parametros;
	instruccion->parametros = parametros;
	return instruccion;
}

void liberar_instruccion(t_instruccion *instruccion) {
	if(instruccion->parametros != NULL){
        for(int i = 0; i < instruccion->cant_parametros; i++){
            free((instruccion->parametros[i]));   
        }
		free(instruccion->parametros);
    }
    free(instruccion);
}

void empaquetar_instruccion(t_paquete *paquete, t_instruccion *i) {
	assert(i->identificador <= 16);								// Existencia del id.
	assert(i->cant_parametros >= 0 && i->cant_parametros <=3);	// Corroboracion de cantidad de parametros.

	agregar_a_paquete(paquete, &(i->identificador), sizeof(uint8_t));
	agregar_a_paquete(paquete, &(i->cant_parametros), sizeof(uint8_t));
	uint cant_bytes;
	if (i->parametros)
		for(int j = 0; j < i->cant_parametros; j++){
			if(i->parametros[j][strlen(i->parametros[j]) - 1] == '\n')
				i->parametros[j][strlen(i->parametros[j]) - 1] = '\0';
			cant_bytes = strlen(i->parametros[j]) + 1;
			
			agregar_a_paquete(paquete, &cant_bytes, sizeof(uint));
			agregar_a_paquete(paquete, i->parametros[j], sizeof(char) * cant_bytes); 
		}
			
}

void imprimir_instruccion(t_instruccion *instruccion) {

	assert(instruccion != NULL);

	uint8_t identificador = instruccion->identificador;
	uint8_t cant_parametros = instruccion->cant_parametros;
	char** parametros = instruccion->parametros;

	printf("\tidentificador: %d\n\tparametros: ", identificador);

	if (parametros == NULL)
		puts("no hay parametros\n");
	else
		for (int i = 0; i < cant_parametros; i++) {
			if (i == cant_parametros - 1)
				printf("%s\n\n", parametros[i]);
			else
				printf("%s, ", parametros[i]);
		}
}

char* get_nombre_instruccion(uint8_t id){
	switch(id){
		case F_WRITE:
			return "F_WRITE";
		break;
		case F_READ:
			return "F_READ";
		break;
		case SET:
			return "SET";
		break;
		case MOV_IN:
			return "MOV_IN";
		break;
		case MOV_OUT:
			return "MOV_OUT";
		break;
		case F_TRUNCATE:
			return "F_TRUNCATE";
		break;
		case F_SEEK:
			return "F_SEEK";
		break;
		case CREATE_SEGMENT:
			return "CREATE_SEGMENT";
		break;
		case IO:
			return "IO";
		break;
		case WAIT:
			return "WAIT";
		break;
		case SIGNAL:
			return "SIGNAL";
		break;
		case F_OPEN:
			return "F_OPEN";
		break;
		case F_CLOSE:
			return "F_CLOSE";
		break;
		case DELETE_SEGMENT:
			return "DELETE_SEGMENT";
		break;
		case EXIT:
			return "EXIT";
		break;
		case YIELD:
			return "YIELD";
		break;
		
		default: 
			return "";	
	}
}

t_instruccion *desempaquetar_instruccion(void *buffer, int *desplazamiento) {
	uint8_t identificador, cant_parametros;
	char** parametros;

	assert(buffer != NULL);

	memcpy(&identificador, buffer + *desplazamiento, sizeof(uint8_t));
	*desplazamiento += sizeof(uint8_t);

	memcpy(&cant_parametros, buffer + *desplazamiento, sizeof(uint8_t));
	*desplazamiento += sizeof(uint8_t);

	if (cant_parametros) {
		
		uint cant_bytes = cant_parametros * sizeof(char*);
		parametros = (char**) malloc(cant_bytes);
        
		for(int i = 0; i < cant_parametros; i++){
			memcpy(&cant_bytes, buffer + *desplazamiento, sizeof(uint));
			*desplazamiento += sizeof(uint);
			parametros[i] = (char*) malloc(cant_bytes);
			memcpy(parametros[i], buffer + *desplazamiento, cant_bytes); 
			*desplazamiento += cant_bytes;
		}
		
	}
	else parametros = NULL;

	return crear_instruccion(identificador, cant_parametros, parametros);
}
void chequear_instrucciones(t_list* lista_instrucciones, int cant_instrucciones){

	t_list_iterator* iterador_lista = list_iterator_create(lista_instrucciones);
	t_instruccion* instruccion_actual; 

	while(list_iterator_has_next(iterador_lista)){
		instruccion_actual = (t_instruccion*) list_iterator_next(iterador_lista);
		
		assert(instruccion_actual != NULL);
		imprimir_instruccion(instruccion_actual);	
	}

	list_iterator_destroy(iterador_lista);
}

void loguear_instruccion(t_log* logger, t_instruccion i, int pid){
	switch (i.cant_parametros)
	{
		case 0: 
			log_info(logger, "PID: %d - Ejecutando: %s", pid, get_nombre_instruccion(i.identificador));
			break;
		case 1: 
			log_info(logger, "PID: %d - Ejecutando: %s - %s", pid, get_nombre_instruccion(i.identificador), i.parametros[0]);
			break;
		case 2: 
			log_info(logger, "PID: %d - Ejecutando: %s - %s %s", pid, get_nombre_instruccion(i.identificador), i.parametros[0], i.parametros[1]);
			break;
		case 3: 
			log_info(logger, "PID: %d - Ejecutando: %s - %s %s %s", pid, get_nombre_instruccion(i.identificador), i.parametros[0], i.parametros[1], i.parametros[2]);
			
	}
}