#include "all_heads.h"


/********************************
 *				*
 *     Variables Globales	*
 *				*
 ********************************/
/* Acceso al nombre con que ejecutaron el programa (argv[0]) */
extern const char	*__progname_full;


/********************************
 *				*
 *      Clase TAnalizadorFS	*
 *				*
 ********************************/
/****************************************************************************************************************************************
 *																	*
 *						    TAnalizadorFS :: TAnalizadorFS							*
 *																	*
 * OBJETIVO: Inicializar la clase recién creada.											*
 *																	*
 * ENTRADA: Nada.															*
 *																	*
 * SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
TAnalizadorFS::TAnalizadorFS()
{
struct winsize WinSize;

/* Inicialziar variables */
LongitudDiskData=0;
DiskData=NULL;
DriverFS=NULL;

/* Levantar el ancho de la pantalla */
ioctl(STDOUT_FILENO, TIOCGWINSZ, &WinSize);

/* Determinar el ancho de las impresiones de buffers en base al ancho de la pantalla */
if (WinSize.ws_col<80)
	PrintWidth=8;
else if (WinSize.ws_col<144)
	PrintWidth=16;
else if (WinSize.ws_col<272)
	PrintWidth=32;
else
	PrintWidth=64;
}


/****************************************************************************************************************************************
 *																	*
 *						    TAnalizadorFS :: ~TAnalizadorFS							*
 *																	*
 * OBJETIVO: Liberar recursos alocados.													*
 *																	*
 * ENTRADA: Nada.															*
 *																	*
 * SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
TAnalizadorFS::~TAnalizadorFS()
{
/* Liberar el driver del filesystem si está cargado */
if (DriverFS)
    {
	delete(DriverFS);
	DriverFS=NULL;
    }

/* Liberar todos los recursos alocados */
BorrarTodoYReinicializar();
}


/****************************************************************************************************************************************
 *																	*
 *						      TAnalizadorFS :: Ejecutar								*
 *																	*
 * OBJETIVO: Esta función ejecuta todos los tests e imprime los resultados.								*
 *																	*
 * ENTRADA: Ruta: Ruta al archivo binario a cargar.											*
 *																	*
 * SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
int TAnalizadorFS::Ejecutar(const char *Ruta)
{
int	CodError;

/* Cargar la imágen de disco */
printf("Cargando imágen de disco ...\n");
if ( (CodError=CargarImagen(Ruta)) != CODERROR_NINGUNO)
	return(CodError);

/* Ver si la imágen es FAT */
printf("Analizando imágen con driver FAT12/FAT16/FAT32 ...\n");
DriverFS=new TDriverFAT(DiskData, LongitudDiskData);
CodError=DriverFS->LevantarDatosSuperbloque();
if ( (CodError==CODERROR_SUPERBLOQUE_INVALIDO) || (CodError==CODERROR_FILESYSTEM_DESCONOCIDO) )
    {
	/* No es FAT, ver si es EXT */
	printf("ERROR: La imágen no es FAT12/FAT16/FAT32.\n");
	delete DriverFS;

	printf("Analizando imágen con driver EXT2/EXT3/EXT4 ...\n");
	DriverFS=new TDriverEXT(DiskData, LongitudDiskData);
	CodError=DriverFS->LevantarDatosSuperbloque();
	if ( (CodError==CODERROR_SUPERBLOQUE_INVALIDO) || (CodError==CODERROR_FILESYSTEM_DESCONOCIDO) )
	    {
		/* No es FAT, ver si es EXT */
		printf("ERROR: La imágen no es EXT2/EXT3/EXT4.\n");
		delete DriverFS;

		printf("Analizando imágen con driver NTFS ...\n");
		DriverFS=new TDriverNTFS(DiskData, LongitudDiskData);
		CodError=DriverFS->LevantarDatosSuperbloque();
		if ( (CodError==CODERROR_SUPERBLOQUE_INVALIDO) || (CodError==CODERROR_FILESYSTEM_DESCONOCIDO) )
			printf("ERROR: La imágen no es NTFS.\n");
	    }
    }
if (CodError!=CODERROR_NINGUNO)
	return(CodError);
printf("ÉXITO: Imágen válida.\n");

/* Mostrar los datos del Filesystem */
DriverFS->MostrarDatosSuperbloque();

/* Ejecutar los tests */
if ( (CodError=EjecutarTests()) != CODERROR_NINGUNO)
	return(CodError);

/* Salir */
return(CODERROR_NINGUNO);
}


/****************************************************************************************************************************************
 *																	*
 *						      TAnalizadorFS :: EjecutarTests							*
 *																	*
 * OBJETIVO: Esta función carga el archivo <nombre-ejecutable>_tests.txt y ejecuta cada uno de los comandos.				*
 *																	*
 * ENTRADA: Ruta: Ruta al archivo binario a cargar.											*
 *																	*
 * SALIDA: En el nombre de la función CODERROR_NINGUNO si no hubo errores, caso contrario el código de error.				*
 *																	*
 ****************************************************************************************************************************************/
int TAnalizadorFS::EjecutarTests()
{
int	CodError = CODERROR_NINGUNO;
char	aux[1024];
char	Delimiters[] = " \t";
char	*p;
FILE	*f;

/* Armar el nombre del archivo de comandos */
sprintf(aux, "%s_tests.txt", __progname_full);

/* Abrir el archivo de comandos */
if ( (f=fopen(aux, "r")) == NULL )
	return(CODERROR_FALTA_ARCHIVO_DE_COMANDOS);

/* Para cada uno de los comandos */
while ( (CodError==CODERROR_NINGUNO) && (fgets(aux, sizeof(aux), f)) )
    {
	/* Sacar los caracteres de fin de línea */
	p=aux+strlen(aux)-1;
	while ( (p>=aux) && ((*p=='\r')||(*p=='\n')) )
		*p--='\0';
	
	/* Si es una línea en blanco o un comentario, saltearla */
	if ( (aux[0]=='\0') || (aux[0]=='#') )
		continue;

	/* Obtener el comando */
	p=strtok(aux, Delimiters);
	if (!strcasecmp(p, "dir"))
	    {
		/* Quieren ejecutar un DIR */

		/* Primero debería venir el directorio */
		p=strtok(NULL, Delimiters);
		if (!p)
			return(CODERROR_COMANDO_CON_ERRORES);
	
		/* Listar el contenido del directorio */
		CodError=MostrarContenidoDirectorio(p);
	    }
	else if (!strcasecmp(p, "cat"))
	    {
		/* Quieren ejecutar un CAT */
		
		/* Primero debería venir la ruta completa al archivo */
		p=strtok(NULL, Delimiters);
		if (!p)
			return(CODERROR_COMANDO_CON_ERRORES);
	
		/* Listar el contenido del directorio */
		CodError=MostrarContenidoArchivo(p);
	    }
	else
	    {
		/* Comando desconocido */
		return(CODERROR_COMANDO_DESCONOCIDO);
	    }
    }

/* Cerrar el archivo de comandos */
fclose(f);

/* Salir indicando éxito */
return(CODERROR_NINGUNO);
}


/****************************************************************************************************************************************
 *																	*
 *						      TAnalizadorFS :: CargarImagen							*
 *																	*
 * OBJETIVO: Esta función carga una nueva imágen de disco para analizar.								*
 *																	*
 * ENTRADA: Ruta: Ruta al archivo binario a cargar.											*
 *																	*
 * SALIDA: En el nombre de la función CODERROR_NINGUNO si no hubo errores, caso contrario el código de error.				*
 *																	*
 ****************************************************************************************************************************************/
int TAnalizadorFS::CargarImagen(const char *Ruta)
{
int		CodError = CODERROR_NINGUNO;
FILE		*f;

/* Voy a cargar una nueva imágen, borrar todo */
BorrarTodoYReinicializar();

/* Abrir el archivo de entrada */
if ( (f=fopen(Ruta, "rb")) == NULL )
    {
	/* No puedo abrirlo, salir */
	CodError = CODERROR_ARCHIVO_INEXISTENTE;
    }
else
    {
	/* Determinar el tamaño */
	fseek(f, 0, SEEK_END);
	LongitudDiskData=ftell(f);
	fseek(f, 0, SEEK_SET);
	
	/* Validar la longitud del archivo */
	if ( (LongitudDiskData%512) != 0 )
	    {
		/* No puede no ser múltiplo de sector */
		CodError = CODERROR_ARCHIVO_INVALIDO;
	    }
	else
	    {
		/* Alocar memoria para almacenarlo */
		if ( (DiskData=(const unsigned char *)malloc(LongitudDiskData)) == NULL )
		    {
			/* No hay suficiente memoria */
			CodError = CODERROR_FALTA_MEMORIA;
		    }
		else
		    {
			/* Levantar el archivo a memoria */
			if ( fread((void *)DiskData, 1, LongitudDiskData, f) != LongitudDiskData )
			    {
				/* Error al levantar el archivo */
				CodError = CODERROR_LECTURA_DISCO;
			    }
		    }
	    }

	/* Cerrar el archivo de entrada */
	fclose(f);
    }

/* Ver si hubo errores */
if (CodError==CODERROR_NINGUNO)
    {
	/* Se cargó sin errores */
	printf("Se cargaron %d bytes de %s sin errores.\n", LongitudDiskData, Ruta);
    }
else
    {
	/* No vale la pena tener cosas por la mitad, borrar todo */
	BorrarTodoYReinicializar();
    }

/* Salir */
return(CodError);
}


/****************************************************************************************************************************************
 *																	*
 *						 TAnalizadorFS :: BorrarTodoYReinicializar						*
 *																	*
 * OBJETIVO: Reinicializar la clase, liberando cualquier recurso alocado, de forma que se pueda iniciar el proceso de otra imágen de	*
 *	     disco.															*
 *																	*
 * ENTRADA: Nada.															*
 *																	*
 * SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
void TAnalizadorFS::BorrarTodoYReinicializar(void)
{
/* Ver si hay imágen cargada */
if (DiskData)
    {
	/* Sí, liberarla */
	free((void *)DiskData);
	LongitudDiskData=0;
	DiskData=NULL;
    }
}


/****************************************************************************************************************************************
 *																	*
 *					   TAnalizadorFS :: MostrarContenidoDirectorio							*
 *																	*
 * OBJETIVO: Esta función usa el driver cargado para listar el contenido de un determinado directorio.					*
 *																	*
 * ENTRADA: Path: Ruta al directorio cuyo contenido listar.										*
 *																	*
 * SALIDA: En el nombre de la función el código de error.										*
 *																	*
 ****************************************************************************************************************************************/
int TAnalizadorFS::MostrarContenidoDirectorio(const char *Path)
{
int				CodError;
std::vector<TEntradaDirectorio> Entradas;

/* Imprimir lo que voy a hacer */
printf("Leyendo directorio '%s' ...\n", Path);

/* Buscar el contenido del directorio */
CodError=DriverFS->ListarDirectorio(Path, Entradas);
if ( (CodError!=CODERROR_NINGUNO) && (CodError!=CODERROR_DIRECTORIO_INEXISTENTE) )
	return(CodError);
if (CodError==CODERROR_NINGUNO)
    {
	/* Lo tengo, mostrarlo por pantalla */
	DriverFS->MostrarDatosDirectorio(Entradas);
    }
else
    {
	/* Si el problema es que el directorio no existe no reportar error, simplemente imprimir que no existe */
	if (CodError==CODERROR_DIRECTORIO_INEXISTENTE)
		printf("\tError, el directorio NO EXISTE!\n");
	else
		return(CodError);
    }

/* Salir indicando éxito */
return(CODERROR_NINGUNO);
}


/****************************************************************************************************************************************
 *																	*
 *					      TAnalizadorFS :: MostrarContenidoArchivo							*
 *																	*
 * OBJETIVO: Esta función usa el driver cargado para listar el contenido de un determinado directorio.					*
 *																	*
 * ENTRADA: Path: Ruta al archivo cuyo contenido listar.										*
 *																	*
 * SALIDA: En el nombre de la función el código de error.										*
 *																	*
 ****************************************************************************************************************************************/
int TAnalizadorFS::MostrarContenidoArchivo(const char *Path)
{
int		CodError;
unsigned	DataLen;
unsigned char	*Data;

/* Imprimir lo que voy a hacer */
printf("Leyendo archivo '%s' ...\n", Path);

/* Buscar el contenido del archivo */
CodError=DriverFS->LeerArchivo(Path, Data, DataLen);
if ( (CodError!=CODERROR_NINGUNO) && (CodError!=CODERROR_ARCHIVO_INEXISTENTE) )
	return(CodError);

if (CodError==CODERROR_NINGUNO)
    {
	/* Lo tengo, mostrarlo por pantalla */
	printf("\tLeído, %u bytes\n", DataLen);
	DriverFS->PrintBuffer(Data, DataLen, PrintWidth);
	free(Data);
    }
else
    {
	/* Si el problema es que el archivo no existe no reportar error, simplemente imprimir que no existe */
	printf("\tError, el archivo NO EXISTE!\n");
    }

/* Salir indicando éxito */
return(CODERROR_NINGUNO);
}


