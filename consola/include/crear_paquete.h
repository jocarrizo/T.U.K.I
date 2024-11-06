#ifndef _CREAR_PAQUETE_H_
#define _CREAR_PAQUETE_H_

#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <instrucciones/instrucciones.h>
#include <sockets/sockets.h>

extern t_log *log_consola;

t_paquete* crear_paquete_instrucciones(FILE* f);

#endif // !_CREAR_PAQUETE_H_
