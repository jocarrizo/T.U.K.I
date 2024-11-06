#include <conexiones_cpu.h>

void inicializar_conexiones(){
    log_info(log_cpu, "Iniciando las conexiones...");
    conectar_memoria(ip_memoria(), puerto_memoria());
    //pthread_create(&conexion_memoria, NULL, (void*)recibirMensajeMemoria, &socket_memoria);

    socket_cpu = iniciar_servidor(puerto_escucha());
    conectar_kernel(socket_cpu);
    
}

void liberar_conexiones(){ // Libera las conexiones con los otros modulos
    liberar_conexion(socket_cpu);
    liberar_conexion(socket_memoria);    
}