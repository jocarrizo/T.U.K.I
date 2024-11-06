#ifndef _CONEXION_KERNEL_H_
#define _CONEXION_KERNEL_H_
#include <sockets/sockets.h>
#include <stdbool.h>
#include <instruccionescpu.h>

extern t_log* log_cpu;
extern t_registros regs;
extern int socket_kernel;
void conectar_kernel(int servidor);

#endif // !_CONEXION_KERNEL_H_
