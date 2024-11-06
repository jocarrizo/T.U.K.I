#include <conexiones_filesystem.h>

void liberar_conexiones(){ // Libera las conexiones con los otros modulos
    liberar_conexion(socket_fs);
    liberar_conexion(socket_memoria);    
}