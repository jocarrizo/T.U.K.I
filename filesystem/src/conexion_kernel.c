#include <conexion_kernel.h>

void conectar_kernel(int servidor){ // Maneja la conexion y comunicacion con el modulo kernel
    socket_kernel = esperar_cliente(servidor);
    bool confirmacion = true;
    send(socket_kernel, &confirmacion, sizeof(bool), 0);
    log_info(log_filesystem, "Se conectó kernel");
    while(true){
        switch(recibir_operacion(socket_kernel)){
            case -1:
                log_error(log_filesystem, "El kernel se desconectó");
                return;
            
            case INSTRUCCIONES_FS:
                t_buffer* buff = crear_buffer();
                buff->stream = recibir_buffer(&(buff->size), socket_kernel);
                pthread_create(&thread_peticion, NULL, (void*) atender_peticion, buff);
                pthread_detach(thread_peticion);
                break;
            
            default:
                log_error(log_filesystem, "Operación Desconocida");
        }        
    }
}