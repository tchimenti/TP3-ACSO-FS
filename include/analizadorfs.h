#ifndef	__ANALIZADORFS__H__
#define	__ANALIZADORFS__H__

/************************
 *			*
 *     Constantes	*
 *			*
 ************************/

/********************************
 *				*
 *      Clase TAnalizadorFS	*
 *				*
 ********************************/
class TAnalizadorFS
{
public:
					TAnalizadorFS();
	virtual				~TAnalizadorFS();
	
	int				Ejecutar(const char *Ruta);

protected:
	unsigned			PrintWidth;
	unsigned			LongitudDiskData;
	const unsigned char		*DiskData;
	TDriverBase			*DriverFS;
	
	virtual int 			EjecutarTests();

	virtual int			CargarImagen(const char *Ruta);
	virtual void			BorrarTodoYReinicializar(void);
	
	virtual int			MostrarContenidoDirectorio(const char *Path);
	virtual int			MostrarContenidoArchivo(const char *Path);
};

#endif
