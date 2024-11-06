#include "manejo_de_filesystem.h"

void crear_fcb(char* nombre_archivo){    
    char* linea = string_new();
    FILE* archivo;
    t_fcb* nuevo_fcb_ = (t_fcb*) malloc(sizeof(t_fcb));
    t_fcb_filesystem* nuevo_fcb = (t_fcb_filesystem*) malloc(sizeof(t_fcb_filesystem));
    nuevo_fcb->fcb = nuevo_fcb_; 
    char* path_mas_nombre = malloc(strlen(fcb()) + strlen(nombre_archivo) + 2);

    snprintf(path_mas_nombre, strlen(fcb()) + strlen(nombre_archivo) + 2, "%s/%s", fcb(), nombre_archivo);
    
    nuevo_fcb_->nombre = nombre_archivo;
    nuevo_fcb_->tam = 0;
    nuevo_fcb_->puntero_directo = 0;
    nuevo_fcb_->puntero_indirecto = 0;
    
    
    
    archivo = fopen(path_mas_nombre, "w+");
    
    string_append_with_format(&linea, "%s=%s\n", "NOMBRE_ARCHIVO", nombre_archivo);
    string_append_with_format(&linea, "%s=%s\n", "TAMANIO_ARCHIVO", "0"); 
    string_append_with_format(&linea, "%s=%s\n", "PUNTERO_DIRECTO", "0"); 
    string_append_with_format(&linea, "%s=%s\n", "PUNTERO_INDIRECTO", "0");

    fwrite(linea, strlen(linea), 1, archivo);   
    
    fclose(archivo);
    nuevo_fcb->fcb_config = config_create(path_mas_nombre);
    list_add(fcbs, nuevo_fcb);
    free(path_mas_nombre);
}

int truncar_fcb(char* id_archivo, int longitud){
    t_fcb_filesystem* fcb_ = getFcb(id_archivo);
    int diff_bloques;
    if(longitud == fcb_->fcb->tam) 
        return longitud;

    if(longitud > fcb_->fcb->tam){
        diff_bloques = ceil((float)(longitud - fcb_->fcb->tam) / block_size());

        if(cant_bloques_libres() < diff_bloques)
            return -1; 
        agregar_bloques(fcb_, diff_bloques);
        fcb_->fcb->tam = longitud;
    }

    if(longitud < fcb_->fcb->tam){
        diff_bloques = floor((float)(fcb_->fcb->tam - longitud) / block_size());
        sacar_bloques(fcb_, diff_bloques);
        fcb_->fcb->tam = longitud;
    }
    actualizar_fcb(fcb_);
    return longitud;
}

void agregar_bloques(t_fcb_filesystem* fcb_, int cant_bloques){
    uint32_t bit = 0;
    bool hubo_acceso = false;
    int cant_punteros = block_size() / sizeof(uint32_t); 
    int tam_puntero = sizeof(uint32_t);
    uint32_t valor_puntero_sin_usar = 0;
    uint32_t valor_puntero;
    

    while(cant_bloques > 0){
        if(bitarray_test_bit(bitmap, bit)){
            log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: %d", bit, bitarray_test_bit(bitmap, bit));
            bit++;
            continue; 
        }
        
        if(fcb_->fcb->puntero_directo == valor_puntero_sin_usar){
            fcb_->fcb->puntero_directo = bit + 1;
            bitarray_set_bit(bitmap, bit);
            log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: 1", bit);
            bit++;
            cant_bloques--;
            continue;
        }
        
        if(fcb_->fcb->puntero_indirecto == valor_puntero_sin_usar){
            fcb_->fcb->puntero_indirecto = bit + 1;

            for(int i = 0; i < cant_punteros; i++){
                memcpy(blocks + bit * block_size() + i * tam_puntero, &valor_puntero_sin_usar, sizeof(tam_puntero));
            }

            
            bitarray_set_bit(bitmap, bit);
            log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: 1", bit);
            bit++;
            continue;
        }

        
        
        for(int i = 0; i < cant_punteros; i++){

            memcpy(&valor_puntero, blocks + (fcb_->fcb->puntero_indirecto - 1) * block_size() + i * tam_puntero, sizeof(tam_puntero));

            if(valor_puntero == valor_puntero_sin_usar){
                
                memcpy(blocks + (fcb_->fcb->puntero_indirecto - 1) * block_size() + i * tam_puntero, &bit, sizeof(tam_puntero));
                bitarray_set_bit(bitmap, bit);
                log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: 1", bit);                
                bit++;
                cant_bloques--;
                hubo_acceso = true;                
                break;
            }
        } 
        
    }
    if(hubo_acceso){
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %d", fcb_->fcb->nombre, fcb_->fcb->puntero_indirecto);
        usleep(retardo_acceso_bloque() * 1000);
    }
        
}

void sacar_bloques(t_fcb_filesystem* fcb_, int cant_bloques){
        uint32_t bit = 0;
        int cant_punteros = block_size() / sizeof(uint32_t); 
        int tam_puntero = sizeof(uint32_t);
        uint32_t valor_puntero;
        uint32_t valor_puntero_sin_usar = 0;
        bool hubo_acceso = false;
        bool hubo_break = false;
        int bloque_puntero_indirecto_fs = fcb_->fcb->puntero_indirecto - 1;

    while(cant_bloques > 0){
        
        if(fcb_->fcb->puntero_indirecto != valor_puntero_sin_usar){
            hubo_break = false;
            for(int i = cant_punteros - 1; i >= 0; i--){
                memcpy(&valor_puntero, blocks + (fcb_->fcb->puntero_indirecto - 1) * block_size() + i * tam_puntero, sizeof(tam_puntero));
                
                if(valor_puntero != valor_puntero_sin_usar){
                    bit = valor_puntero;
                    bitarray_clean_bit(bitmap, bit);
                    log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: 0", bit);
                    cant_bloques--;
                    hubo_acceso = true;  
                    memcpy(blocks + (fcb_->fcb->puntero_indirecto - 1) * block_size() + i * tam_puntero, &valor_puntero_sin_usar, sizeof(tam_puntero));                    
                    hubo_break = true;
                    break;
                }
            }
            if(!hubo_break){
                bit = fcb_->fcb->puntero_indirecto - 1;
                fcb_->fcb->puntero_indirecto = valor_puntero_sin_usar;
                bitarray_clean_bit(bitmap, bit);
                log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: 0", bit);
                
            }
            continue;

        }
        bit = fcb_->fcb->puntero_directo - 1;
        fcb_->fcb->puntero_directo = valor_puntero_sin_usar;
        bitarray_clean_bit(bitmap, bit);
        log_info(log_filesystem, "Acceso a Bitmap - Bloque: %d - Estado: 0", bit);
        break;       
    }
    
    if(hubo_acceso){
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %d", fcb_->fcb->nombre, bloque_puntero_indirecto_fs);
        usleep(retardo_acceso_bloque() * 1000);
    }
            
}

//PROBAR
void actualizar_fcb(t_fcb_filesystem* fcb_){
    char* tam = string_new();
    char* puntero_directo = string_new();
    char* puntero_indirecto = string_new();
    string_append_with_format(&tam, "%d", fcb_->fcb->tam);
    string_append_with_format(&puntero_directo, "%d", fcb_->fcb->puntero_directo);
    string_append_with_format(&puntero_indirecto, "%d", fcb_->fcb->puntero_indirecto);
    
    config_set_value(fcb_->fcb_config, "TAMANIO_ARCHIVO", tam);
    config_set_value(fcb_->fcb_config, "PUNTERO_DIRECTO", puntero_directo);
    config_set_value(fcb_->fcb_config, "PUNTERO_INDIRECTO", puntero_indirecto);
    config_save(fcb_->fcb_config);
    free(tam);
    free(puntero_directo);
    free(puntero_indirecto); 
}

void sincronizar_bitmap(){
    msync(mapeo_bitmap_en_memoria, ceil((float)block_count() / 8), MS_SYNC | MS_INVALIDATE);
}

void sincronizar_bloques(){
    msync(blocks, block_count() * block_size(), MS_SYNC | MS_INVALIDATE);
}

t_fcb_filesystem* getFcb(char* nombre_archivo){
    t_list_iterator* iterador_fcbs = list_iterator_create(fcbs);
    t_fcb_filesystem* siguiente = NULL;    
    int indice = iterador_fcbs->index;

    while(list_iterator_has_next(iterador_fcbs)){
        siguiente = list_iterator_next(iterador_fcbs);    
        indice = iterador_fcbs->index;    
        if(strcmp(siguiente->fcb->nombre, nombre_archivo) == 0){
            list_iterator_destroy(iterador_fcbs);
            return siguiente;
        }
                    
    }
    list_iterator_destroy(iterador_fcbs);
    
    if(indice != -1 && strcmp(siguiente->fcb->nombre, nombre_archivo) == 0)
        return siguiente;
    return NULL;
}

int cant_bloques_libres(){
    int cantidad = 0;
    int cant_total_bloques = block_count();
    for(int i = 0; i < cant_total_bloques; i++){
        if(!bitarray_test_bit(bitmap, i))
            cantidad++;
    }
    return cantidad;
}

void* leer_de_archivo(char* id_archivo ,int puntero_de_archivo ,int cant_bytes_a_acceder) {

    t_fcb_filesystem* fcb_ = getFcb(id_archivo);
    int aux1; 
    int bloque_accedido = 0;
    int desplazamiento = 0;
    int cant_de_accesos_bloque = 1;
    int bloque_accedido_fs = fcb_->fcb->puntero_directo - 1;
    void* valord = NULL;
    t_buffer* buffer = crear_buffer();

    bloque_accedido  = puntero_de_archivo/block_size();

    desplazamiento = puntero_de_archivo%block_size();

    
    if(bloque_accedido == 0 && desplazamiento + cant_bytes_a_acceder <= block_size()){
        
        valord = malloc(cant_bytes_a_acceder);
        memcpy(valord, blocks + bloque_accedido_fs * block_size() + desplazamiento, cant_bytes_a_acceder);
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %d", id_archivo, bloque_accedido_fs);
    }    

    
    if(bloque_accedido == 0 && desplazamiento + cant_bytes_a_acceder > block_size()){
       
        aux1 = block_size() - desplazamiento; 
        valord = malloc(aux1);
        memcpy(valord, blocks + bloque_accedido_fs * block_size() + desplazamiento, aux1);
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %d", id_archivo, bloque_accedido_fs);
        agregar_a_buffer(buffer, valord, aux1);
        free(valord);
        bloque_accedido = 2;
        cant_bytes_a_acceder -= aux1;
        desplazamiento = 0;
        cant_de_accesos_bloque++;
    }

    if (bloque_accedido != 0 ){
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %d", id_archivo, fcb_->fcb->puntero_indirecto - 1);
        while(cant_bytes_a_acceder > 0){
            (cant_bytes_a_acceder - (block_size() - desplazamiento)) >= 0 ? (aux1 = block_size() - desplazamiento) : (aux1 = cant_bytes_a_acceder);
            
            memcpy(&bloque_accedido_fs, blocks + (fcb_->fcb->puntero_indirecto - 1) * block_size() + (bloque_accedido - 2) * sizeof(uint32_t), sizeof(uint32_t));
            valord = malloc(aux1);
            memcpy(valord, blocks + bloque_accedido_fs * block_size() + desplazamiento, aux1);
            log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", id_archivo, bloque_accedido, bloque_accedido_fs);
            agregar_a_buffer(buffer, valord, aux1);
            free(valord);
            cant_bytes_a_acceder -= aux1;
            cant_de_accesos_bloque++;
            bloque_accedido++;
            desplazamiento = 0;
        }
        valord = malloc(buffer->size);
        memcpy(valord, buffer->stream, buffer->size);        
    }

    liberar_buffer(buffer);
    usleep(retardo_acceso_bloque() * cant_de_accesos_bloque * 1000);    
    return valord;
}

void escribir_en_archivo(char* id_archivo ,int puntero_de_archivo ,char* valord, int cant_bytes_a_acceder) {

    t_fcb_filesystem* fcb_ = getFcb(id_archivo);

    int escritos = 0;
    int aux1;
    int bloque_accedido;
    int desplazamiento;
    int cant_de_accesos_bloque = 1;
    int bloque_accedido_fs = fcb_->fcb->puntero_directo - 1;
    
    bloque_accedido  = puntero_de_archivo / block_size();

    desplazamiento = puntero_de_archivo % block_size();

    if(bloque_accedido == 0 && desplazamiento + cant_bytes_a_acceder <= block_size()){
        memcpy(blocks + bloque_accedido_fs * block_size() + desplazamiento, valord, cant_bytes_a_acceder);
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %d", id_archivo, bloque_accedido_fs);
    }

    if(bloque_accedido == 0 && desplazamiento + cant_bytes_a_acceder > block_size()){
        escritos = block_size() - desplazamiento;
        memcpy(blocks + bloque_accedido_fs * block_size() + desplazamiento, valord, escritos);
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %d", id_archivo, bloque_accedido_fs);
        bloque_accedido = 2;
        desplazamiento = 0;
        cant_bytes_a_acceder -= escritos;
        cant_de_accesos_bloque++;
    }

    if (bloque_accedido != 0){
        log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %d", id_archivo, fcb_->fcb->puntero_indirecto - 1);
         while(cant_bytes_a_acceder > 0){
            (cant_bytes_a_acceder - (block_size() - desplazamiento)) >= 0 ? (aux1 = block_size() - desplazamiento) : (aux1 = cant_bytes_a_acceder);
            memcpy(&bloque_accedido_fs, blocks + (fcb_->fcb->puntero_indirecto - 1) * block_size() + (bloque_accedido - 2) * sizeof(uint32_t), sizeof(uint32_t));
            memcpy(blocks + bloque_accedido_fs * block_size() + desplazamiento, valord + escritos, aux1);
            log_info(log_filesystem, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", id_archivo, bloque_accedido, bloque_accedido_fs);
            cant_bytes_a_acceder -= aux1;
            escritos += aux1;
            desplazamiento = 0;
            bloque_accedido++;
            cant_de_accesos_bloque++;        
        }        
    }

    usleep(retardo_acceso_bloque() * cant_de_accesos_bloque * 1000);
    free(valord);
}