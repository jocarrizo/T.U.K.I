#include "manejo_memoria_usuario.h"

// INICIALIZACION DE SEMAFOROS MUTEX
void inicializarSemaforo(){
    sem_init(&memoria_usuario_sem, 0, 1);
    sem_init(&tablas_de_segmentos_sem, 0, 1);
    sem_init(&huecos_libres_sem, 0, 1); 
}

int crear_segmento(int tam, int pid, int id){
    int base;
    t_tabla_de_segmentos* tabla;
    base = hay_hueco_libre(tam);
    if(base >= 0){
        t_segmento* seg = (t_segmento*) malloc(sizeof(t_segmento));
        seg->base = base;
        seg->tamanio = tam;
        actualizar_huecos(base, tam, OCUPA);
        sem_wait(&tablas_de_segmentos_sem);
        tabla = list_get(tablas_de_segmentos, get_index_tabla(pid)); 
        sem_post(&tablas_de_segmentos_sem);  
        tabla->segmentos[id] = seg;    
    }        
    return base;
}

bool orden_ascendente(void* a, void* b){
    t_tabla_de_segmentos* seg1 = (t_tabla_de_segmentos*) a;
    t_tabla_de_segmentos* seg2 = (t_tabla_de_segmentos*) b;
    return seg1->pid < seg2->pid;
}

int get_index_tabla(int pid){
    int index;
    t_tabla_de_segmentos* tabla;
    t_list_iterator* iterador_tablas = list_iterator_create(tablas_de_segmentos);
    while(list_iterator_has_next(iterador_tablas)){
        tabla = list_iterator_next(iterador_tablas);
        if(tabla->pid == pid)
            break;        
                  
    }
    index = list_iterator_index(iterador_tablas);
    list_iterator_destroy(iterador_tablas);
    return index;
}

void borrar_segmento(t_tabla_de_segmentos* tabla, int id){
    actualizar_huecos(tabla->segmentos[id]->base, tabla->segmentos[id]->tamanio, LIBERA);
    liberar_segmento(tabla->segmentos[id]);
    tabla->segmentos[id] = NULL;
}

void actualizar_huecos(int base, int tam, op_hueco op){
    if(op == LIBERA){
        if(chekearHuecosContiguos(base , tam)){//chekeo si hay huecos contiguos -> sino lo agrego directo a la lista
            agregarHuecoLibre(base,tam);
        }
    }
    if(op == OCUPA){
        sem_wait(&huecos_libres_sem);        
        t_hueco_libre* hueco = list_get(huecos_libres, get_index_hueco(base));
        sem_post(&huecos_libres_sem);
        if(hueco->fin - hueco->inicio + 1 > tam) //ver porq  + 1
            hueco -> inicio = base + tam;
        else{
            sem_wait(&huecos_libres_sem);
            list_remove_element(huecos_libres, hueco);
            sem_post(&huecos_libres_sem);
            free(hueco);
        }            
    }
}

//    ---- ELIMINAS UN ESPACIO DE MEMORIA Y CREAS UN HUECO LIBRE ----
void agregarHuecoLibre(int base, int tam){ 
    t_hueco_libre *nuevoHueco = (t_hueco_libre*) malloc(sizeof(t_hueco_libre));
    int finaliza;
    finaliza = base + tam - 1;
    // crear hueco libre
    nuevoHueco->inicio = base;
    nuevoHueco->fin = finaliza; 
    // agregar hueco a la lista de huecos libres
    sem_wait(&huecos_libres_sem);
    list_add(huecos_libres, nuevoHueco); 
    sem_post(&huecos_libres_sem);
}
int chekearHuecosContiguos(int base, int tam){ 
    int i = 0;
    int final;
    int noHayContiguos = 1;
    final = base + tam - 1;
    int hayQueConsolidarInferior = -1 ,hayQueConsolidarSuperior = -1;
    t_hueco_libre *huecoInf;
    t_hueco_libre *huecoSup;
    sem_wait(&huecos_libres_sem);
    for(i=0; i<list_size(huecos_libres); i++){
        t_hueco_libre *hueco = list_get(huecos_libres,i);

        if((hueco->inicio - final) == 1){ 
        noHayContiguos = 0;
        hayQueConsolidarInferior = i;
        } // chequeo contiguidad por abajo
        
        if((base - hueco->fin) == 1){ 
        noHayContiguos = 0;
        hayQueConsolidarSuperior = i;
        } // chequeo contiguidad por arriba
    }
    if(hayQueConsolidarInferior >= 0 && hayQueConsolidarSuperior >= 0)
    {
        huecoInf = list_get(huecos_libres, hayQueConsolidarInferior);
        huecoSup = list_remove(huecos_libres,hayQueConsolidarSuperior);
        huecoInf -> inicio = huecoSup -> inicio;
        free(huecoSup);
    }
    else if(hayQueConsolidarInferior >= 0) {
        huecoInf = list_get(huecos_libres,hayQueConsolidarInferior);
        consolidarInferior(base,huecoInf);
    }
    else if(hayQueConsolidarSuperior >= 0) {
        huecoSup = list_get(huecos_libres,hayQueConsolidarSuperior);
        consolidarSuperior(final,huecoSup);
    }
    sem_post(&huecos_libres_sem);
     return noHayContiguos;
}
void consolidarInferior(int base , t_hueco_libre* inferior ){ // CAMBIAR DEFINICION
    inferior->inicio = base;
}
void consolidarSuperior(int final, t_hueco_libre* superior){ // CAMBIAR DEFINICION
    superior->fin = final;
}

int get_index_hueco(int base){
    int index;
    t_hueco_libre* hueco;
    t_list_iterator* iterador_huecos_libres = list_iterator_create(huecos_libres);
    while(list_iterator_has_next(iterador_huecos_libres)){
        hueco = list_iterator_next(iterador_huecos_libres);
        if(hueco->inicio == base){
            index = list_iterator_index(iterador_huecos_libres);
            list_iterator_destroy(iterador_huecos_libres);     
            return index;
        }        
    }
    if(hueco->inicio == base){
        index = list_iterator_index(iterador_huecos_libres);
        list_iterator_destroy(iterador_huecos_libres);     
        return index;
    }        
    list_iterator_destroy(iterador_huecos_libres);
    return -1;
}

int hay_hueco_libre(int tam){ // RECIBE UN TAMANIO Y DEVUELVE UNA BASE DONDE COLOCAR EL SEGMENTO
        if(strcmp(algoritmo_asignacion(),"FIRST")==0){return firstFit(tam);}
        if(strcmp(algoritmo_asignacion(),"BEST")== 0){return bestFit(tam);}
        if(strcmp(algoritmo_asignacion(),"WORST")== 0){return worstFit(tam);}
    return 0;
}
//    ---- ASIGNAS EL SEGMENTO A UN HUECO LIBRE SEGUN EL ALGORITMO QUE CORRESPONDA ----
int firstFit(int tam){ //agarra el primer hueco libre y lo ocupa
    int tamanio_hueco;
    int tamanios_sumados = 0;
    t_hueco_libre* hueco;
    sem_wait(&huecos_libres_sem);
    t_list_iterator* iterador_huecos_libres = list_iterator_create(huecos_libres);
    while(list_iterator_has_next(iterador_huecos_libres)){
        hueco = list_iterator_next(iterador_huecos_libres);
        tamanio_hueco = hueco->fin - hueco->inicio + 1;
        if(tamanio_hueco >= tam){
            list_iterator_destroy(iterador_huecos_libres);
            sem_post(&huecos_libres_sem);
            return hueco->inicio; // SE ENCONTRO UN HUECO LIBRE QUE CUMPLE
        }
        tamanios_sumados += tamanio_hueco;           
    }
    list_iterator_destroy(iterador_huecos_libres);
    sem_post(&huecos_libres_sem);
    if(tamanios_sumados >= tam)
        return -1; // NO SE COLOCO PERO HAY ESPACIO SUFICIENTE --> HAY QUE COMPACTAR
    return -2; // NO HAY ESPACIO SUFICIENTE 
}
int bestFit(int tam){
    sem_wait(&huecos_libres_sem);
    list_sort(huecos_libres,menorAMayor);
    sem_post(&huecos_libres_sem);
    return firstFit(tam);
}
int worstFit(int tam){
    sem_wait(&huecos_libres_sem);
    list_sort(huecos_libres,mayorAMenor);
    sem_post(&huecos_libres_sem);
    return firstFit(tam);
}

// FUNCIONES PARA ORDENAR POR TAMANIO LA LISTA DE HUECOS LIBRES

bool mayorAMenor(void* h1,void* h2){
    t_hueco_libre * hueco = (t_hueco_libre*) h1;
    t_hueco_libre * hueco2 = (t_hueco_libre*) h2;
    return (hueco->fin - hueco->inicio) > (hueco2->fin - hueco2->inicio);
}
bool menorAMayor(void* h1,void* h2){
    t_hueco_libre * hueco = (t_hueco_libre*) h1;
    t_hueco_libre * hueco2 = (t_hueco_libre*) h2;
    return (hueco->fin - hueco->inicio) < (hueco2->fin - hueco2->inicio);
}

void enviar_base(int base, int socket_a_enviar){
    t_paquete* paquete = crear_paquete(BASE_SEGMENTO);
    agregar_a_paquete(paquete, &base, sizeof(int));
    enviar_paquete(paquete, socket_a_enviar);
}

// ----------- COMPACTACION ----------- 
void compactacion (){
int i=0;
t_tabla_de_segmentos* tabla; 
t_list* lista_segmentos = list_create(); //CONSULTAR --> hacerla global
    for(i=0;i<list_size(tablas_de_segmentos);i++)
    {
    sem_wait(&tablas_de_segmentos_sem);
    tabla = list_get(tablas_de_segmentos, i); 
    sem_post(&tablas_de_segmentos_sem); 
    crearListaSegmentos(tabla, &lista_segmentos);
    }
    list_add(lista_segmentos, segmento_0);
    ordenarSegmentosPorBase(&lista_segmentos);
    iniciarCompactacion(&lista_segmentos);//CONSULTAR --> &lista_segmentos
}   
void crearListaSegmentos(t_tabla_de_segmentos* tabla,t_list** lista_segmentos){
    for(int i=1;i<tabla->tam;i++){
        if(tabla->segmentos[i]!= NULL)
            list_add(*lista_segmentos, tabla->segmentos[i]);    
    }
}
void ordenarSegmentosPorBase(t_list** lista_segmentos){
    list_sort(*lista_segmentos,menorAMayorBase);
}
bool menorAMayorBase(void* s1,void* s2){
    return ((t_segmento*)s1)->base < ((t_segmento*)s2)->base;
}

void iniciarCompactacion(t_list** lista_segmentos){
    int i ;

    for(i=1;i<list_size(*lista_segmentos);i++)
    {
    t_segmento* segmento = list_get(*lista_segmentos,i);
    t_segmento* segmento_anterior = list_get(*lista_segmentos,i-1);
        
    memcpy(memoria_usuario + segmento_anterior->base + segmento_anterior->tamanio ,memoria_usuario + segmento->base , segmento->tamanio);
    segmento ->base = segmento_anterior->base + segmento_anterior->tamanio;
    
    }
    t_segmento* ultimo_segmento = list_get(*lista_segmentos, i - 1);
    actualizar_huecos_compactado(ultimo_segmento ->base + ultimo_segmento->tamanio);
    list_destroy(*lista_segmentos);
}

void actualizar_huecos_compactado(int inicio){
     t_hueco_libre *nuevoHueco = (t_hueco_libre*) malloc(sizeof(t_hueco_libre));
     nuevoHueco->inicio = inicio;
     nuevoHueco->fin = tam_memoria() - 1;
    list_clean_and_destroy_elements(huecos_libres, free);
    list_add(huecos_libres,nuevoHueco);
}

void enviarTablasSegmentos(int socket_kernel){
    t_paquete * paquete = crear_paquete(TABLAS_DE_SEGMENTOS);
    for(int i=0;i<list_size(tablas_de_segmentos);i++){
        empaquetar_tabla_segs(paquete,list_get(tablas_de_segmentos,i));
    }
    enviar_paquete(paquete,socket_kernel);
}

