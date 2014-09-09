Los scripts ansisop corren abstraidos del proceso programa mediante el siguiente soft link.

> ln -s /RUTA_DEL_PROCESO_PROGRAMA /usr/bin/ansisop

Con SÓLO indicarles la ruta del interprete (/usr/bin/ansisop) ALCANZA !!! :D


RECORDAR:
	- DAR permisos a scripts para su ejecucion.
	- UBICACION del fichero 'config.cfg' (ANSISOP_CONFIG)
	- EN caso de no dar con la tecla con la var LD_LIBRARY_PATH hacer un soft link por c/u de las shared libraries