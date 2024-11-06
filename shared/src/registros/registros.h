#ifndef _REGISTROS_H_
#define _REGISTROS_H_
#include <string.h>
#include <stdlib.h>

typedef struct{
	char AX[4], BX[4], CX[4], DX[4];
	char EAX[8], EBX[8], ECX[8], EDX[8];
	char RAX[16], RBX[16], RCX[16], RDX[16];
}t_registros;

int get_tam_reg(char*);
void asignar_registro(char*, char*, t_registros*);
char* get_valor_reg(char*, t_registros*);
#endif // !_REGISTROS_H_
