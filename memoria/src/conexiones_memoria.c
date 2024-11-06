#include <conexiones_memoria.h>

int inicializarConexiones(){ // Inicia puerto de escucha y maneja las conexiones y comunicacion con los otros modulos
    int socket_memoria = -1, socket_cliente = -1, socket_fs = -1, socket_kernel = -1, socket_cpu = -1;
    socket_memoria = iniciar_servidor(puerto_escucha());       
    if(socket_memoria == -1){
        log_error(log_memoria,"No se pudo crear el servidor");       
        return EXIT_FAILURE;
    }
    log_info(log_memoria,"Servidor iniciado");
    
    pthread_t comunicacion_kernel;
    pthread_t comunicacion_cpu;
    pthread_t comunicacion_fs;

    for(int i = 0; i < 3; i++){
        socket_cliente = esperar_cliente(socket_memoria);
        int id = recibir_operacion(socket_cliente);
        if(id == 0){
            log_info(log_memoria,"Se conectó kernel");            
            socket_kernel = socket_cliente;
            pthread_create(&comunicacion_kernel, NULL, (void*)conectar_con_kernel, &socket_kernel);
        } 
        if(id == 1){
            log_info(log_memoria,"Se conectó filesystem");            
            socket_fs = socket_cliente;
            pthread_create(&comunicacion_fs, NULL, (void*)conectar_con_fs, &socket_fs);  
        } 
        if(id == 2){
            log_info(log_memoria,"Se conectó cpu");
            socket_cpu = socket_cliente;
            pthread_create(&comunicacion_cpu, NULL, (void*)conectar_con_cpu, &socket_cpu);  
        } 
                
    }
    pthread_join(comunicacion_cpu, NULL);
    pthread_join(comunicacion_fs, NULL);
    pthread_join(comunicacion_kernel, NULL);
    liberar_conexion(socket_memoria);
    return EXIT_SUCCESS;
}

void* conectar_con_cpu(int* socket_cpu){ // Realiza handshake con cpu y queda a la espera de mensajes
    bool confirmacion = true;
    int codigo;
    send(*socket_cpu, &confirmacion, sizeof(bool), 0);
    while(true){
        codigo = recibir_operacion(*socket_cpu);
        if(codigo == -1){
            log_error(log_memoria, "El cpu se desconectó");
            break;
        }

        if(codigo != INSTRUCCIONES_MEMORIA){
            log_error(log_memoria, "Operación Desconocida");
            int error = -1;
            send(*socket_cpu, &error, sizeof(int), 0);
            continue;
        }
        t_buffer* buff = crear_buffer();
        buff->stream = recibir_buffer(&(buff->size), *socket_cpu);
        realizar_peticion(buff, "CPU", *socket_cpu);               
    }
    return NULL;
}

void* conectar_con_fs(int* socket_fs){ // Realiza handshake con filesystem y queda a la espera de mensajes 
    bool confirmacion = true;
    int codigo;
    send(*socket_fs, &confirmacion, sizeof(bool), 0);    
    while(true){
        codigo = recibir_operacion(*socket_fs);
        if(codigo == -1){
            log_error(log_memoria, "El filesystem se desconectó");
            break;
        }

        if(codigo != INSTRUCCIONES_MEMORIA){
            log_error(log_memoria, "Operación Desconocida");
            int error = -1;
            send(*socket_fs, &error, sizeof(int), 0);
            continue;
        }
        t_buffer* buff = crear_buffer();
        buff->stream = recibir_buffer(&(buff->size), *socket_fs);
        realizar_peticion(buff, "FS", *socket_fs); 
    }
    return NULL;
}

void* conectar_con_kernel(int* socket_kernel){ // Realiza handshake con kernel y queda a la espera de mensajes   
    bool confirmacion = true;
    int codigo;
    send(*socket_kernel, &confirmacion, sizeof(bool), 0);    
    while(true){
        codigo = recibir_operacion(*socket_kernel); 
        if(codigo == -1){
            log_error(log_memoria, "El kernel se desconectó");
            break;
        }
       
        if(codigo != INSTRUCCIONES_MEMORIA){
            log_error(log_memoria, "Operación Desconocida");
            int error = -1;
            send(*socket_kernel, &error, sizeof(int), 0);
            continue;
        }

        t_buffer* buff = crear_buffer();
        buff->stream = recibir_buffer(&(buff->size), *socket_kernel);
        realizar_peticion(buff, "KERNEL", *socket_kernel); 
    }
    return NULL;
}