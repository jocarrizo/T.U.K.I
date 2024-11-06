#include "pcb.h"

void liberar_pcb(t_pcb *pcb) {
	liberar_contexto(pcb->contexto_ejecucion);
	free(pcb);
}