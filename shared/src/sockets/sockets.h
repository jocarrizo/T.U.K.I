#ifndef SOCKETS_H_
#define SOCKETS_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

//REVER
typedef enum {
	INSTRUCCIONES_CONSOLA, INSTRUCCIONES_MEMORIA, INSTRUCCIONES_FS, MENSAJE, TERMINAR_CONSOLA, CONTEXTO_EJECUCION, ERROR, SEG_FAULT, YIELD_, EXIT_, 
	CREATE_SEGMENT_, DELETE_SEGMENT_ , SIGNAL_, WAIT_, IO_, F_OPEN_, F_CLOSE_, F_SEEK_, F_TRUNCATE_, F_READ_, F_WRITE_,	TABLA_SEGMENTOS, 
	BASE_SEGMENTO, TABLAS_DE_SEGMENTOS, OUT_OF_MEMORY, PEDIDO_COMPACTACION, TABLA_SEGMENTOS_ACTUALIZADA, APERTURA, CREACION, TRUNCAMIENTO, FCB, ESCRITURA_FS_OK, LECTURA_FS_OK
} op_code;

typedef struct {
	int size;
	void *stream;
} t_buffer;

//REVER
typedef struct {
	int codigo_operacion;
	t_buffer *buffer;
} t_paquete;

struct addrinfo *addrinfo_servidor(char *ip, char *puerto);
int crear_socket(struct addrinfo *server_info);


// servidor

int iniciar_servidor(char *puerto);
int esperar_cliente(int);
void *recibir_buffer(int *, int);
int recibir_operacion(int);
void *recibir_mensaje(int);

// cliente

int crear_conexion(char *ip, char *puerto);
t_buffer *crear_buffer();
void liberar_buffer(t_buffer*);
t_paquete *crear_paquete(int codigo_operacion);
void agregar_a_paquete(t_paquete *paquete, void *valor, int bytes);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void enviar_mensaje2(void *mensaje, int bytes, int socket_cliente);
void enviar_mensaje(void *mensaje, int bytes, int socket_cliente);
void liberar_conexion(int socket_cliente);
void *sacar_de_buffer(t_buffer *buffer, int tam_dato);
void agregar_a_buffer(t_buffer* buffer, void* valor, int bytes);

#endif /* SOCKETS_H_ */