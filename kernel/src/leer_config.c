#include <leer_config.h>

void crear_config(char* config_path) {
	config = config_create(config_path);
}

void destruir_config() {
	config_destroy(config);
}

// memoria
char *ip_memoria() {
	return config_get_string_value(config, "IP_MEMORIA");
}
char *puerto_memoria() {
	return config_get_string_value(config, "PUERTO_MEMORIA");
}

// cpu
char *ip_cpu() {
	return config_get_string_value(config, "IP_CPU");
}
char *puerto_cpu() {
	return config_get_string_value(config, "PUERTO_CPU");
}

// filesystem
char* ip_filesystem(){
	return config_get_string_value(config, "IP_FILESYSTEM");
}
char* puerto_filesystem(){
	return config_get_string_value(config, "PUERTO_FILESYSTEM");
}


// kernel
char *puerto_escucha() {
	return config_get_string_value(config, "PUERTO_ESCUCHA");
}

// planificacion
char *algoritmo_planificacion() {
	return config_get_string_value(config, "ALGORITMO_PLANIFICACION");
}
int grado_multiprogramacion() {
	return config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");
}

// hrrn
double estimacion_inicial() {
	return config_get_double_value(config, "ESTIMACION_INICIAL");
}
double alfa() {
	return config_get_double_value(config, "HRRN_ALFA");
}

// recursos
char **recursos_config() {
	return config_get_array_value(config, "RECURSOS");
}

char **instancias_recursos() {
	return config_get_array_value(config, "INSTANCIAS_RECURSOS");
}
//AGREGAR FUNCIONES PARA LEER LO Q FALTA