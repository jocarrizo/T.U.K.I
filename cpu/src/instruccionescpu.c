#include <instruccionescpu.h>

void ciclo_de_instruccion(t_contexto_ejecucion* contexto){  
    tiempo_real_ejecucion = temporal_create();
   	t_instruccion instruccion_actual;	
    regs = contexto->registros;
    direccion_fisica = 0;
    num_segmento = 0;
	while (contexto != NULL) {
		instruccion_actual = fetch(contexto);		
		if (decode(instruccion_actual)){                        
            int dir_log = 0;            
            if(instruccion_actual.identificador == MOV_IN || instruccion_actual.identificador == F_READ || instruccion_actual.identificador == F_WRITE)
                dir_log = 1;        
            
            num_segmento = floor(atoi(instruccion_actual.parametros[dir_log]) / tam_max_segmento());
            desplazamiento_segmento = atoi(instruccion_actual.parametros[dir_log]) % tam_max_segmento();            

            seg_trabajo = contexto->tabla_segmentos->segmentos[num_segmento];
            
            direccion_fisica = seg_trabajo->base + desplazamiento_segmento;            
        }

        loguear_instruccion(log_cpu, instruccion_actual, contexto->id);
        execute(instruccion_actual, &contexto);				
	}    
}

t_instruccion fetch(t_contexto_ejecucion* contexto){
    t_instruccion instruccion_nueva;
	instruccion_nueva = *((t_instruccion*) list_get(contexto->instrucciones, contexto->program_counter));	
	contexto->program_counter++;
	return instruccion_nueva;
}

bool decode(t_instruccion instruccion){
    return instruccion.identificador == MOV_IN || instruccion.identificador == MOV_OUT || instruccion.identificador == F_READ || instruccion.identificador == F_WRITE;    
}

void execute(t_instruccion instruccion_actual, t_contexto_ejecucion** contexto){
    int tam_a_acceder;
    char* valor;
    switch (instruccion_actual.identificador){
    case SET:
        asignar_registro(instruccion_actual.parametros[0], instruccion_actual.parametros[1], &regs);
        
        usleep(retardo_instruccion() * 1000);
        break;
    
    case YIELD:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, YIELD_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case IO:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, IO_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);        
        break;

    case WAIT:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, WAIT_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);         
        break;


    case SIGNAL:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, SIGNAL_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);                         
        break;

    case EXIT:
        (*contexto)->registros = regs;
        
        enviar_contexto(*contexto, socket_kernel, EXIT_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case MOV_IN:
        tam_a_acceder = get_tam_reg(instruccion_actual.parametros[0]);
        
        if( seg_trabajo == NULL || desplazamiento_segmento + tam_a_acceder > seg_trabajo->tamanio){
            log_info(log_cpu, "PID: %d - Error SEG_FAULT - Segmento: %d - Offset: %d - Tamaño: %d", (*contexto)->id, num_segmento, desplazamiento_segmento, seg_trabajo->tamanio);                
            temporal_stop(tiempo_real_ejecucion);
            temporal_destroy(tiempo_real_ejecucion);
            enviar_contexto(*contexto, socket_kernel, SEG_FAULT);
            liberar_contexto(*contexto);
            *contexto = NULL;
            break;                
        }
        valor = leer_de_memoria(direccion_fisica, tam_a_acceder, (*contexto) -> id, socket_memoria);
        asignar_registro(instruccion_actual.parametros[0], valor, &regs);
        free(valor);
        valor = get_valor_reg(instruccion_actual.parametros[0], &regs);
        log_info(log_cpu, "PID: %d - Acción: LEER - Segmento: %d - Dirección Física: %d - Valor: %s", (*contexto)->id, num_segmento, direccion_fisica, valor);
        free(valor);
        break;

    case MOV_OUT:
        tam_a_acceder = get_tam_reg(instruccion_actual.parametros[1]);        
        
        if(seg_trabajo == NULL || desplazamiento_segmento + tam_a_acceder > seg_trabajo->tamanio){
            log_info(log_cpu, "PID: %d - Error SEG_FAULT - Segmento: %d - Offset: %d - Tamaño: %d", (*contexto)->id, num_segmento, desplazamiento_segmento, seg_trabajo->tamanio);                
            temporal_stop(tiempo_real_ejecucion);
            temporal_destroy(tiempo_real_ejecucion);
            enviar_contexto(*contexto, socket_kernel, SEG_FAULT);
            liberar_contexto(*contexto);
            *contexto = NULL;
            break;                
        }
        valor = get_valor_reg(instruccion_actual.parametros[1], &regs);
        escribir_en_memoria(direccion_fisica, valor, tam_a_acceder, (*contexto)->id, socket_memoria);
        log_info(log_cpu, "PID: %d - Acción: ESCRIBIR - Segmento: %d - Dirección Física: %d - Valor: %s", (*contexto)->id, num_segmento, direccion_fisica, valor);
        free(valor);
        break;
    
    case CREATE_SEGMENT:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, CREATE_SEGMENT_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case DELETE_SEGMENT:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, DELETE_SEGMENT_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case F_OPEN:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, F_OPEN_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case F_CLOSE:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, F_CLOSE_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case F_SEEK:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, F_SEEK_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case F_TRUNCATE:
        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto(*contexto, socket_kernel, F_TRUNCATE_);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;
    case F_READ:
        
        if( seg_trabajo == NULL || desplazamiento_segmento + atoi(instruccion_actual.parametros[2]) > seg_trabajo->tamanio){
            log_info(log_cpu, "PID: %d - Error SEG_FAULT - Segmento: %d - Offset: %d - Tamaño: %d", (*contexto)->id, num_segmento, desplazamiento_segmento, seg_trabajo->tamanio);                
            temporal_stop(tiempo_real_ejecucion);
            temporal_destroy(tiempo_real_ejecucion);
            enviar_contexto(*contexto, socket_kernel, SEG_FAULT);
            liberar_contexto(*contexto);
            *contexto = NULL;
            break;                
        }

        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);

        enviar_contexto_read_write(*contexto, socket_kernel, F_READ_, direccion_fisica);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    case F_WRITE:

        if( seg_trabajo == NULL || desplazamiento_segmento + atoi(instruccion_actual.parametros[2]) > seg_trabajo->tamanio){
            log_info(log_cpu, "PID: %d - Error SEG_FAULT - Segmento: %d - Offset: %d - Tamaño: %d", (*contexto)->id, num_segmento, desplazamiento_segmento, seg_trabajo->tamanio);                
            temporal_stop(tiempo_real_ejecucion);
            temporal_destroy(tiempo_real_ejecucion);
            enviar_contexto(*contexto, socket_kernel, SEG_FAULT);
            liberar_contexto(*contexto);
            *contexto = NULL;
            break;                
        }

        (*contexto)->registros = regs;
        temporal_stop(tiempo_real_ejecucion);
        (*contexto)->tiempo_real_ej += (double) temporal_gettime(tiempo_real_ejecucion);
        
        enviar_contexto_read_write(*contexto, socket_kernel, F_WRITE_, direccion_fisica);
        liberar_contexto(*contexto);
        *contexto = NULL;
        temporal_destroy(tiempo_real_ejecucion);
        break;

    default:
        break;
    }
}


void enviar_contexto_read_write(t_contexto_ejecucion* contexto, int socket_a_enviar, op_code codigo, int direccion_fisica){
    t_paquete* paquete = crear_paquete(codigo);
    empaquetar_contexto(paquete, contexto);
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    enviar_paquete(paquete, socket_a_enviar);
}