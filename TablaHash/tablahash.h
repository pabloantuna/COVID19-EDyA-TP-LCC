#ifndef __TABLAHASH_H__
#define __TABLAHASH_H__

#include <stdio.h>
#include "../RList/rlist.h"
#include <stdlib.h>
#include <string.h>
#define TAMANO 163

typedef struct {
  RList *Tabla;
  unsigned numElems;
  unsigned capacidad;
} Tabla;

typedef Tabla *TablaHash;
typedef void (*Destruir)(void *dato);
typedef unsigned long (*FuncionHash)(void *dato);
typedef void (*FuncionVisitante)(void *dato);
typedef void (*FuncionImprimir)(void *dato, FILE * archivo);
typedef int (*Compara)(void *dato1, void *dato2);

/**
 * Crea una tabla hash
 */
TablaHash tabla_crear(unsigned size);

/**
 * Dada una tabla tabla la destruye e inicializa
 * una nueva tabla vacia con la misma capacidad
 */
TablaHash tabla_reiniciar(TablaHash tabla);

/**
 * Dada una tabla y un tamaño para una nueva tabla
 * elimina la tabla dada luego de mover las listas
 * de registros a una nueva tabla donde tendran otra
 * clave de hasheo
 */
TablaHash tabla_resize(TablaHash tabla, unsigned nuevoSize);

/**
 * Toma una dato, el tamaño de la tabla y una funcion de hasheo
 * y devuelve el resto de aplicarle la funcion al dato por el tamaño
 * siendo este resultado una clave de la tabla hash
 */
unsigned crear_clave(unsigned size, void *dato, FuncionHash fun);

/**
 * Crea una clave a partir del departamento y la localidad
 */
unsigned crear_clave_fdl(unsigned size, char *dpto, char *localidad);

/**
 * Dada una tabla, el nombre de un archivo de salida y una cadena
 * imprime la tabla en el archivo de nombre pasado imprimiendo como cabecera
 * en el mismo la cadena pasada como argumento.
 */
void tabla_imprimir(TablaHash tabla, char *nombreArchivo, char *cabecera);

/**
 * Dada una tabla la destruye por completo
 */
void tabla_destruir(TablaHash tabla);

/**
 * Dada una tabla y un registro inserta el registro
 * en la lista que le corresponde o lo reemplaza si ya existia
 */
TablaHash tabla_insertar_reemplazar(TablaHash tabla, Registro dato);

/**
 * Dada una tabla, una fecha, una localidad y un departamento
 * elimina de la tabla al registro con esa fecha localidad y departamento
 */
TablaHash tabla_eliminar_by_fld(TablaHash tabla, char *fecha, char *localidad,
                                char *dpto);

/**
 * Dada una tabla un departamente y una localidad
 * busca el pico de casos diarios en dicho departamento
 * y localidad en los registros de la tabla
 */
int tabla_buscar_pico(TablaHash tabla, char *dpto, char *localidad);

/**
 * Dada una tabla, una fecha, un departamente y una localidad
 * busca la cantidad de casos acumulados hasta la fehca dada 
 * en el departamento y localidad  dados en los registros de la tabla
 */
int tabla_buscar_acumulado(TablaHash tabla, char *fecha, char *dpto,
                           char *localidad);

/**
 * Dada una tabla una fecha, un departamente y una localidad
 * estima el tiempo de duplicacion de casos acumulados para la fecha dada
 * en el departamento y localidad  dados en los registros de la tabla
 */
int tabla_tiempo_duplicacion(TablaHash tabla, char *fecha, char *dpto,
                             char *localidad);

/**
 * Dada una tabla, una fecha de inicio, una fecha de fin, un departamento, y una localidad
 * realiza la grafica de los casos acumulados y la grafica de los casos diarios
 * entre las fechas dadas para el departamento y localidad dados en los
 * registros de la tabla
 */
void tabla_graficar(TablaHash tabla, char *fechaInicio, char *fechaFin,
                    char *dpto, char *localidad);

#endif
