#include <crear_paquete.h>


void empaquetar_linea(t_paquete *paquete, char **instruccion) {
	
	uint8_t identificador = get_identificador(instruccion[0]);

	if (identificador == NO_RECONOCIDO) {
		log_error(log_consola, "Identificador \"%s\" no reconocido", instruccion[0]);
		return;
	}

	uint8_t cant_parametros = get_cant_parametros(identificador);
	char** parametros;
   
	if (!cant_parametros) parametros = NULL;
	else {
		parametros = (char**) malloc(sizeof(char*) * cant_parametros);
       
		for (int j = 0; j < cant_parametros; j++){
			int cant_bytes = strlen(instruccion[j+1]);
			parametros[j] = (char *) malloc(sizeof(char) * cant_bytes + 1);
			if(cant_bytes == 1)
				parametros[j][0] = instruccion[j+1][0];			
			else
				for(int k = 0; k < cant_bytes; k++)
					parametros[j][k] = instruccion[j+1][k];
				//memcpy(&(parametros[j]), &(instruccion[j+1]), cant_bytes);
			parametros[j][cant_bytes] = '\0'; 
		}
	}

	t_instruccion *i = crear_instruccion(identificador, cant_parametros, parametros);
	empaquetar_instruccion(paquete, i);
	//imprimir_instruccion(i);
	liberar_instruccion(i);
}

t_paquete* crear_paquete_instrucciones(FILE* f) {
	log_info(log_consola, "Creando paquete de instrucciones...");
	t_paquete *paquete = crear_paquete(INSTRUCCIONES_CONSOLA);

	char *linea = NULL;
	size_t lon = 0;

	log_info(log_consola, "Leyendo instrucciones de archivo...");
	while (getline(&linea, &lon, f) != -1) {
		empaquetar_linea(paquete, string_split(linea, " "));
	}

	fclose(f);
	free(linea);

	log_info(log_consola, "Archivo de instrucciones leído correctamente");
	log_info(log_consola, "Paquete creado exitosamente");
	return paquete;
}