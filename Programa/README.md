Proceso Programa
================

**DESCRIPCION** (sujeta a cambios)
		
Se encarga de recibir un stream proveniente de un script (ansisop) y enviarlo mediante socket al proceso Kernel (específicamente al PLP). Además, este proceso va a mostrar por pantalla todo lo que devuelva el CPU por medio del Parser.

**CICLO DE VIDA DEL PROGRAMA**

 * 1- Crear/Abrir file log del programa para el registro de actividades
 * 2- Leer archivo de configuracion
 * 3- Conectar a Kernel
 * 4- Abrir script (pasado implicitamente como param)
 * 5- Enviar script al Kernel
 * 6- Quedar a la espera de mensajes para imprimir

**REGISTRO EN FILE.LOG** (NO DISPONIBLE AUN para no mezclar con el codigo funcional)

A modo de despejar cualquier inquietud, la funcion del file log, como la de cualquier otro log, es la de contener un registro detallado de las actividades 
que hace nuestro programa.
En las commons, la cátedra nos provee las funciones para el manejo y volcado de estos registros (log.c/log.h).
En estas, existe (entre tantas) una funcion para CREAR un log file llamada 'log_create()', ésta recibe 4 params:
1) ubicacion (ruta) del log file 
2) nombre del programa que se va a ejecutar ('calcular_factorial.ansisop' por decir alguno)
3) booleano true/false para indicar si se quiere imprimir por pantalla lo que loggea.
4) nivel de log, hay distintos (LOG_LEVEL_TRACE,	LOG_LEVEL_DEBUG,	LOG_LEVEL_INFO,LOG_LEVEL_WARNING,	LOG_LEVEL_ERROR)

Con solo instanciar el logger de esta forma alcanza, si se piensa correr OTRO programa, uno distinto del anterior, el log volverá a registrar...pero claro, con 
ocurrencias del nuevo programa y en una nueva linea.
Es por esto, que en la medida que nuestro programa se tope con ciertas ocurrencias las irá registrando en el file log.
Una vez que el programa deja de registrar en el file log (sea por alguna interrupcion I/O, inactividad del programa, etc) se debe destruir la instancia del 
logger con el consecuente cierre de archivo y liberacion de memoria.

Ruta del log ->  /tp-2014-1c-david-y-goliat/Programa/file.log

**CAMBIOS RELEVANTES**

(06/05/2014 ) Se agrega búsqueda del contenido de la variable de entornot ANSISOP_CONFIG.
Esta contiene la ruta del archivo de configuracion, info que debe ser parseada por este proceso (programa)
La GRAN duda es si ya se va a encontrar definida o si hay que setearla a mano...ya se preguntará, se deja comentado !! !

(06/05/2014) Se chequea que el 2do argumento recibido sea un script .ansisop

	**LINKS**

https://github.com/sisoputnfrba/tp-2014-1c-david-y-goliat
https://github.com/sisoputnfrba/so-commons-library
