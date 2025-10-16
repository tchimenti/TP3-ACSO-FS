#include "all_heads.h"
#include "all_heads.h"


/********************************
 *				*
 *      Clase TDriverBase	*
 *				*
 ********************************/
/****************************************************************************************************************************************
 *																	*
 *						      TDriverBase :: TDriverBase							*
 *																	*
 * OBJETIVO: Inicializar la clase recién creada.											*
 *																	*
 * ENTRADA: DiskData: Puntero a un bloque de memoria con la imágen del disco a analizar.						*
 *	    LongitudDiskData: Tamaño, en bytes, de la imágen a analizar.								*
 *																	*
 * SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
TDriverBase::TDriverBase(const unsigned char *DiskData, unsigned LongitudDiskData)
{
/* Tomar los valores recibidos */
TDriverBase::DiskData=DiskData;
TDriverBase::LongitudDiskData=LongitudDiskData;

/* Inicialziar variables */
memset(&DatosFS, 0, sizeof(DatosFS));
}


/****************************************************************************************************************************************
 *																	*
 *						     TDriverBase :: ~TDriverBase							*
 *																	*
 * OBJETIVO: Liberar recursos alocados.													*
 *																	*
 * ENTRADA: Nada.															*
 *																	*
 * SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
TDriverBase::~TDriverBase()
{
}


/****************************************************************************************************************************************
 *																	*
 *						   TDriverBase :: PunteroASector							*
 *																	*
 * OBJETIVO: Esta función devuelve un puntero al sector pedido.										*
 *																	*
 * ENTRADA: NroSector: Número de sector (el primero es el sector es el 0).								*
 *																	*
 * SALIDA: En el nombre de la función el puntero a los datos del sector.								*
 *																	*
 * OBSERVACIONES: IMPORTANTE: Esta función sólo puede usarse para acceder al sector 0 hasta tanto se inicialice la variable 		*
 *			      DatosFS.BytesPorSector.											*
 *																	*
 ****************************************************************************************************************************************/
const unsigned char *TDriverBase::PunteroASector(__u64 NroSector)
{
/* Ver si tengo imágen cargada */
if (!DiskData)
	return(NULL);

/* Ver si el sector existe */
if ( ((NroSector+1)*DatosFS.BytesPorSector) >= LongitudDiskData )
    {
	/* No, la imágen cargada no tiene tantos sectores */
	return(NULL);
    }

/* Retornar el puntero solicitado */
return(DiskData+NroSector*DatosFS.BytesPorSector);
}


/****************************************************************************************************************************************
 *																	*
 *						    TDriverBase :: MostrarDatosSuperbloque						*
 *																	*
 * OBJETIVO: Esta función muestra los datos del superbloque.										*
 *																	*
 * ENTRADA: Nada.															*
 *																	*
 * SALIDA: En el nombre de la función CODERROR_NINGUNO si no hubo errores, caso contrario el código de error.				*
 *																	*
 * OBSERVACIONES: Si llega acá LevantarDatosSuperbloque() retornó sin errores, por lo que el TipoFilesystem es uno de los tipos 	*
 *		  soportados.														*
 *																	*
 ****************************************************************************************************************************************/
int TDriverBase::MostrarDatosSuperbloque(void)
{
unsigned	i;

/* Mostrar los datos que tenga */
printf("Datos del superbloque:\n");

/* Tipo de Filesystem */
switch (DatosFS.TipoFilesystem)
    {
	case tfsFAT12:
		printf("\tFormato                 : FAT12\n");
		break;
	case tfsFAT16:
		printf("\tFormato                 : FAT16\n");
		break;
	case tfsFAT32:
		printf("\tFormato                 : FAT32\n");
		break;
	case tfsEXT2:
		printf("\tFormato                 : EXT2\n");
		break;
	case tfsEXT3:
		printf("\tFormato                 : EXT3\n");
		break;
	case tfsEXT4:
		printf("\tFormato                 : EXT4\n");
		break;
    }

/* Mostrar los valores comunes a todos los Filesystems */
printf("\tBytes/Sector            : %d\n", DatosFS.BytesPorSector);
printf("\tBytes/Cluster           : %d\n", DatosFS.BytesPorCluster);
printf("\tNro Clusters            : %d\n", DatosFS.NumeroDeClusters);

/* Mostrar los valores propios de cada Filesystem */
switch (DatosFS.TipoFilesystem)
    {
	case tfsFAT12:
	case tfsFAT16:
	case tfsFAT32:
		printf("\tNro Sectores            : %d\n", DatosFS.DatosEspecificos.FAT.TotalSectores);
		printf("\tNro Sectores Ocultos    : %d\n", DatosFS.DatosEspecificos.FAT.SectoresOcultos);
		printf("\tNro Sectores Reservados : %d\n", DatosFS.DatosEspecificos.FAT.SectoresReservados);
		printf("\tNro copias FAT          : %d\n", DatosFS.DatosEspecificos.FAT.CopiasFAT);
		printf("\tNro entradas RootDir    : %d\n", DatosFS.DatosEspecificos.FAT.EntradasRootDir);
		printf("\tSectores/Cluster        : %d\n", DatosFS.DatosEspecificos.FAT.SectoresPorCluster);
		printf("\tSectores/FAT            : %d\n", DatosFS.DatosEspecificos.FAT.SectoresPorFAT);
		printf("\tNro Clusters RootDir    : %d\n", DatosFS.DatosEspecificos.FAT.ClustersRootDir);
		printf("\t1er Cluster RootDir     : %d\n", DatosFS.DatosEspecificos.FAT.PrimerClusterRootDir);
		break;
	case tfsEXT2:
	case tfsEXT3:
	case tfsEXT4:
		printf("\tCaract. Compatibles     : %04X\n", DatosFS.DatosEspecificos.EXT.CaracteristicasCompatibles);
		printf("\tCaract. Incompatibles   : %04X\n", DatosFS.DatosEspecificos.EXT.CaracteristicasIncompatibles);
		printf("\tCaract. Sólo Lectura    : %04X\n", DatosFS.DatosEspecificos.EXT.CaracteristicasSoloLectura);
		printf("\tClusters/Grupo          : %d\n", DatosFS.DatosEspecificos.EXT.ClustersPorGrupo);
		printf("\tINodes/Grupo            : %d\n", DatosFS.DatosEspecificos.EXT.INodesPorGrupo);
		printf("\tBytes/INode             : %d\n", DatosFS.DatosEspecificos.EXT.BytesPorINode);
		printf("\tNro INodes              : %d\n", DatosFS.DatosEspecificos.EXT.NumeroDeINodes);
		printf("\tNro Grupos              : %d\n", DatosFS.DatosEspecificos.EXT.NroGrupos);
		printf("\tPeríodo Agrupado Flex   : %d\n", DatosFS.DatosEspecificos.EXT.PeriodoAgrupadoFlex);
		printf("\tNro Clust. reserv. GDT  : %d\n", DatosFS.DatosEspecificos.EXT.ClustersReservadosGDT);
		printf("\tCluster Bitmap INodes   : ");
		for(i=0;i<DatosFS.DatosEspecificos.EXT.NroGrupos;i++)
			printf("%llu%s", DatosFS.DatosEspecificos.EXT.DatosGrupo[i].ClusterBitmapINodes, i!=(DatosFS.DatosEspecificos.EXT.NroGrupos-1)? ", ":"\n");
		printf("\tCluster Tabla INodes    : ");
		for(i=0;i<DatosFS.DatosEspecificos.EXT.NroGrupos;i++)
			printf("%llu%s", DatosFS.DatosEspecificos.EXT.DatosGrupo[i].ClusterTablaINodes, i!=(DatosFS.DatosEspecificos.EXT.NroGrupos-1)? ", ":"\n");
		printf("\tCluster Bitmap Bloques  : ");
		for(i=0;i<DatosFS.DatosEspecificos.EXT.NroGrupos;i++)
			printf("%llu%s", DatosFS.DatosEspecificos.EXT.DatosGrupo[i].ClusterBitmapBloques, i!=(DatosFS.DatosEspecificos.EXT.NroGrupos-1)? ", ":"\n");
		printf("\tCluster Tabla Bloques   : ");
		for(i=0;i<DatosFS.DatosEspecificos.EXT.NroGrupos;i++)
			printf("%llu%s", DatosFS.DatosEspecificos.EXT.DatosGrupo[i].ClusterTablaBloques, i!=(DatosFS.DatosEspecificos.EXT.NroGrupos-1)? ", ":"\n");
		break;
	case tfsNTFS:
		printf("\tOffset Part en Sectores : %d\n", DatosFS.DatosEspecificos.NTFS.OffsetParticionEnSectores);
		printf("\tNro Sectores            : %lld\n", DatosFS.DatosEspecificos.NTFS.TotalSectores);
		printf("\tSectores/Cluster        : %d\n", DatosFS.DatosEspecificos.NTFS.SectoresPorCluster);
		printf("\tBytes/FileRecordSegment : %d\n", DatosFS.DatosEspecificos.NTFS.BytesPorFileRecordSegment);
		printf("\tBytes/IndexBuffer       : %d\n", DatosFS.DatosEspecificos.NTFS.BytesPorIndexBuffer);
		printf("\tNro Cluster $MFT        : %llu\n", DatosFS.DatosEspecificos.NTFS.ClusterMFT);
		printf("\tNro Cluster $MFT Mirror : %llu\n", DatosFS.DatosEspecificos.NTFS.ClusterMFTMirror);
		break;
    }

/* Salir */
return(CODERROR_NINGUNO);
}


/****************************************************************************************************************************************
 *																	*
 *						     TDriverBase :: MostrarDatosDirectorio						*
 *																	*
 * OBJETIVO: Esta función muestra en contenido de un directorio.									*
 *																	*
 * ENTRADA: Entradas: Arreglo de estructuras con las entradas (archivos/directorios/etc) a mostrar.					*
 *																	*
 * SALIDA: En el nombre de la función CODERROR_NINGUNO si no hubo errores, caso contrario el código de error.				*
 *																	*
 ****************************************************************************************************************************************/
int TDriverBase::MostrarDatosDirectorio(std::vector<TEntradaDirectorio> &Entradas)
{
char	aux[65];
int	i;
tm	*LocalTime;

/* Primer fila del encabezado */
printf(" Fecha Creación   Fecha Ult Acceso  Fecha Ult Modif                                Nombre                                 Flags     Tamaño  ");
switch(DatosFS.TipoFilesystem)
    {
	case tfsFAT12:
	case tfsFAT16:
	case tfsFAT32:
		printf("   1º Clu  ");
		break;
	case tfsEXT2:
	case tfsEXT3:
	case tfsEXT4:
		printf("    INode   ");
		break;
	case tfsNTFS:
		printf("   Índice MFT     Sec ");
		break;
    }
printf("\n");

/* Segunda fila del encabezado */
printf("----------------- ----------------- ----------------- ---------------------------------------------------------------- ----------- ----------");
switch(DatosFS.TipoFilesystem)
    {
	case tfsFAT12:
	case tfsFAT16:
	case tfsFAT32:
		printf(" ----------");
		break;
	case tfsEXT2:
	case tfsEXT3:
	case tfsEXT4:
		printf(" -----------");
		break;
	case tfsNTFS:
		printf(" --------------- ----");
		break;
    }
printf("\n");

/* Para cada entrada de directorio a mostrar */
for(i=0;i<Entradas.size();i++)
    {
	/* Mostrar la fecha de creación */
	if (!Entradas[i].FechaCreacion)
	    {
		/* No tengo esa fecha, dejarla en blanco */
		printf("                  ");
	    }
	else
	    {
		/* Convertir el valor a HMS */
		LocalTime=localtime(&Entradas[i].FechaCreacion);
		printf("%02d/%02d/%04d %02d:%02d  ", LocalTime->tm_mday, 1+LocalTime->tm_mon, 1900+LocalTime->tm_year, LocalTime->tm_hour, LocalTime->tm_min);
	    }

	/* Mostrar la fecha de último acceso */
	if (!Entradas[i].FechaUltimoAcceso)
	    {
		/* No tengo esa fecha, dejarla en blanco */
		printf("                  ");
	    }
	else
	    {
		/* Convertir el valor a HMS */
		LocalTime=localtime(&Entradas[i].FechaUltimoAcceso);
		printf("%02d/%02d/%04d %02d:%02d  ", LocalTime->tm_mday, 1+LocalTime->tm_mon, 1900+LocalTime->tm_year, LocalTime->tm_hour, LocalTime->tm_min);
	    }
	
	/* Mostrar la fecha de última modificación */
	if (!Entradas[i].FechaUltimaModificacion)
	    {
		/* No tengo esa fecha, dejarla en blanco */
		printf("                  ");
	    }
	else
	    {
		/* Convertir el valor a HMS */
		LocalTime=localtime(&Entradas[i].FechaUltimaModificacion);
		printf("%02d/%02d/%04d %02d:%02d  ", LocalTime->tm_mday, 1+LocalTime->tm_mon, 1900+LocalTime->tm_year, LocalTime->tm_hour, LocalTime->tm_min);
	    }

	/* Mostrar el nombre */
	strncpy(aux, Entradas[i].Nombre.c_str(), 64);
	aux[64]='\0';
	printf("%64s", aux);
	
	/* Imprimir los flags */
	printf(" ");
	printf("%c", Entradas[i].Flags&fedSOLO_LECTURA     ? 'R' : ' ');
	printf("%c", Entradas[i].Flags&fedOCULTO           ? 'H' : ' ');
	printf("%c", Entradas[i].Flags&fedSISTEMA          ? 'S' : ' ');
	printf("%c", Entradas[i].Flags&fedETIQUETA_VOLUMEN ? 'V' : ' ');
	printf("%c", Entradas[i].Flags&fedDIRECTORIO       ? 'D' : ' ');
	printf("%c", Entradas[i].Flags&fedARCHIVAR         ? 'A' : ' ');
	printf("%c", Entradas[i].Flags&fedACCESO_DIRECTO   ? 'L' : ' ');
	printf("%c", Entradas[i].Flags&fedCOMPRIMIDO       ? 'C' : ' ');
	printf("%c", Entradas[i].Flags&fedENCRIPTADO       ? 'E' : ' ');
	printf("%c", Entradas[i].Flags&fedDISPERSO         ? 'P' : ' ');
	printf(" ");


	/* Colocar el tamaño */
	printf(" %10llu", Entradas[i].Bytes);

	/* Mostrar columnas FS dependientes */
	switch(DatosFS.TipoFilesystem)
	    {
		case tfsFAT12:
		case tfsFAT16:
		case tfsFAT32:
			/* Colocar el primer cluster */
			printf(" %10u", Entradas[i].DatosEspecificos.FAT.PrimerCluster);
			break;
		case tfsEXT2:
		case tfsEXT3:
		case tfsEXT4:
			printf(" %11u", Entradas[i].DatosEspecificos.EXT.INode);
			break;
		case tfsNTFS:
			printf(" %15llu", Entradas[i].DatosEspecificos.NTFS.IndiceMFT);
			printf(" %04X", Entradas[i].DatosEspecificos.NTFS.NroSecuencia);
	    }

	/* Cerrar la línea */
	printf("\n");
    }

/* Salir */
return(CODERROR_NINGUNO);
}


/****************************************************************************************************************************************
 *																	*
 *						     TDriverBase :: MostrarDatosDirectorio						*
 *																	*
 *  OBJETIVO: Esta función imprime en contenido de un buffer binario en pantalla.							*
 *																	*
 *  ENTRADA: Buffer: Puntero al bloque binario.												*
 *	     BufferLen: Longitud del bloque a imprimir.											*
 *																	*
 *  SALIDA: Nada.															*
 *																	*
 ****************************************************************************************************************************************/
void TDriverBase::PrintBuffer(const unsigned char *Buffer, unsigned BufferLen, unsigned BytesPorLinea)
{
int	i, j;

/* Inicializar la salida */
i=0;
while (i<BufferLen)
    {
	/* Indentar la línea */
	printf("    %08x    ", i);

	/* Tomar un bloque de BytesPorLinea caracteres e imprimirlo como hexa */
	for(j=i;j<(i+BytesPorLinea);j++)
	    {
		if (j<BufferLen)
			printf("%02X ", Buffer[j]);
		else
			printf("   ");
	    }

	/* Tomar el mismo bloque e imprimirlo como caracteres */
	for(j=i;j<(i+BytesPorLinea);j++)
	    {
		if (j<BufferLen)
		    {
			if ( (Buffer[j]>=' ')||((Buffer[j]<0)) )
				printf("%c", Buffer[j]);
			else
				printf(".");
		    }
		else
		    {
			printf(" ");
		    }
	    }

	/* Cerrar la línea */
	printf("\n");

	/* Pasar al siguiente bloque */
	i+=BytesPorLinea;
    }
}









