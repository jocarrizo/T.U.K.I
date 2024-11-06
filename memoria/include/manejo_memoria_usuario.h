#ifndef _MANEJO_MEMORIA_USUARIO_H_
#define _MANEJO_MEMORIA_USUARIO_H_
#include <sockets/sockets.h>
#include <segmentos/segmentos.h>
#include <leer_config.h>

// CORRESPONDIENTE A LOS SEMAFOROS
#include <semaphore.h>

//IMPLEMENTAR MUTEX PARA LAS VARIABLES COMPARTIDAS (MEMORIA_USUARIO, TABLAS_DE_SEGMENTOS, HUECOS_LIBRES, SEGMENTO_0)
//Semaforos
extern sem_t memoria_usuario_sem;
extern sem_t tablas_de_segmentos_sem;
extern sem_t huecos_libres_sem;



typedef struct{
    int inicio;
    int fin;
}t_hueco_libre;

typedef enum{
    LIBERA, OCUPA
}op_hueco;

extern t_config* config;
extern t_list* tablas_de_segmentos;
extern t_list* huecos_libres;
extern t_segmento* segmento_0;
extern void* memoria_usuario;

void inicializarSemaforo();
bool orden_ascendente(void*, void*);
int crear_segmento(int, int, int);
int get_index_tabla(int);
void borrar_segmento(t_tabla_de_segmentos*, int);
void actualizar_huecos(int, int, op_hueco); 
int get_index_hueco(int);
int hay_hueco_libre(int );
void enviar_base(int, int); //ENVIA LA BASE, -1 O -2. (-1 == REQUIERE COMPACTAR) (-2 == OUT OF MEMORY)

void agregarHuecoLibre(int base, int tam);
int chekearHuecosContiguos(int base, int tam);
void consolidarInferior(int base , t_hueco_libre* inferior );
void consolidarSuperior(int final, t_hueco_libre* superior);

int hay_hueco_libre(int tam);
int firstFit(int tam);
int bestFit(int tam);
int worstFit(int tam);
bool mayorAMenor(void * hueco,void * hueco2);

bool menorAMayor(void * hueco,void * hueco2);

// ----- COMPACTACION -----
void compactacion ();
void crearListaSegmentos(t_tabla_de_segmentos* tabla,t_list** lista_segmentos);
void ordenarSegmentosPorBase(t_list** lista_segmentos);
bool menorAMayorBase(void* s1,void* s2);
void iniciarCompactacion(t_list** lista_segmentos);
void actualizar_huecos_compactado(int inicio);
void enviarTablasSegmentos(int socket_kernel);
#endif // !_MANEJO_MEMORIA_USUARIO_H_