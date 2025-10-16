#include "all_heads.h"

int main(int argc, char *argv[])
{
int		CodError;
TAnalizadorFS	AnalizadorFS;

/* Analizar los parámetros */
if (argc!=2)
	return(CODERROR_PARAMETROS_INVALIDOS);

/* Ejeuctar la clase que busca el driver adecuado y luego analiza la imágen */
CodError=AnalizadorFS.Ejecutar(argv[1]);

/* Imprimir un mensaje final */
printf("El programa termina con resultado %d.\r\n", CodError);

/* Salir */
return(CodError);
}
