#include <conexion_kernel.h>

void conectar_kernel(int servidor){ // Maneja la conexion y comunicacion con el modulo kernel
    socket_kernel = esperar_cliente(servidor);
    bool confirmacion = true;
    send(socket_kernel, &confirmacion, sizeof(bool), 0);
    log_info(log_cpu, "Se conectó kernel");
    t_buffer* buffer; 
    t_contexto_ejecucion* contexto;
    
     while(true){
        log_info(log_cpu,"No hay contexto, esperando...");
        switch(recibir_operacion(socket_kernel)){
            case -1:
                log_error(log_cpu, "El kernel se desconectó");
                return;
        
            case CONTEXTO_EJECUCION:
                log_info(log_cpu, "Llego un contexto");
                break;
            
            default:
                log_info(log_cpu, "Operación no reconocida");
                continue;                
        }        
        buffer = crear_buffer();
        buffer-> stream = recibir_buffer(&(buffer -> size), socket_kernel);        
        contexto = desempaquetar_contexto(buffer->stream);
        liberar_buffer(buffer);
        log_info(log_cpu,"contexto obtenido!!!");      
        ciclo_de_instruccion(contexto);
    } 
}