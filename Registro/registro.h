#ifndef __REGISTRO_H__
#define __REGISTRO_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct {
  char *fecha;
  char *departamento;
  char *localidad;
  int confirmados;
  int descartados;
  int enEstudio;
  int notificaciones;
} Celda;

typedef Celda *Registro;

/**
 * registro_crear: char* -> char* -> char* -> int -> int -> int -> int -> Registro
 * Dados los datos de un registro crea uno nuevo con dichos datos
 * y lo devuelve
 */
Registro registro_crear(char *fecha, char *dpto, char *localidad, int confirm,
                        int descart, int estudio, int notificaciones);

/**
 * registro_is_number_no_neg: char* -> int
 * Dada una cadena de caracteres esta funcion
 * devuelve un 1 si la cadena representa un numero
 * no negativo y un 0 en caso de ser negativo o no 
 * ser numero
 */
int registro_is_number_no_neg(char *dato);

/**
 * registro_fecha_valida: char* -> int
 * Dada una cadena de caracteres esta funcion
 * devuelve un 1 si es el formato de fecha esperado
 * 0 en caso contrario
 */
int registro_fecha_valida(char *fecha);

/**
 * registro_crear_from_string: char* -> int -> Registro
 * Dado una cadena con el formato esperado (fecha,departamente,....)
 * y un entero que indica si en la cadena se pasa o no las notificaciones totales
 * devuelve un registro nuevo con los datos de la cadena
 */
Registro registro_crear_from_string(char *entrada, int incluyeNotificaciones);

/**
 * registro_destruir: Registro
 * Dado un registro lo destruye
 */
void registro_destruir(Registro reg);

/**
 * registro_hash_fdl: char* -> char* -> unsigned long
 * Dado un departamento y una localidad
 * devuelve una clave de la combinacion de ambas cadenas
 */
unsigned long registro_hash_fdl(char *dpto, char *localidad);

/**
 * registro_string_clave_hash: void* -> unsigned long
 * Dado un puntero a void lo castea a char*
 * y se devuelve una clave de esa cadena
 * (motivo por el cual toma void* es porque debe ser FuncionHash)
 */
unsigned long registro_string_clave_hash(void *dato);

/**
 * registro_clave_hash: void* -> unsigned long
 * Dado un puntero a void lo castea a Registro
 * y se devuelve una clave de la combinacion
 * de las cadenas de departamento y localidad de ese registro
 * (motivo por el cual toma void* es porque debe ser FuncionHash)
 */
unsigned long registro_clave_hash(void *dato);

/**
 * registro_comparar: Registro -> Registro -> int
 * compara dos registros pasados como argumento
 * en base a su fecha, departamento y localidad
 * devolviendo 0 si son iguales o 1 en caso contrario
 */
int registro_comparar(Registro reg1, Registro reg2);

/**
 * registro_comparar_fld: Registro -> char* -> char* -> char* -> int
 * compara un registro pasado como argumento
 * con la fecha, departamento y localidad dados como argumentos
 * devolviendo 0 si son iguales o 1 en caso contrario
 */
int registro_comparar_fld(Registro reg, char *fecha, char *loc, char *dpto);

/**
 * registro_comparar_ld: Registro -> char* -> char* -> int
 * compara un registro pasado como argumento
 * con el departamento y localidad dados como argumentos
 * devolviendo 0 si son iguales o 1 en caso contrario
 */
int registro_comparar_ld(Registro reg, char *loc, char *dpto);

/**
 * registro_comparar_fechas: char* -> char* -> int
 * Dadas dos cadenas con el formato de fecha utilizado
 * compara las fechas devolviendo
 * -1, 0, 1 si la primera es menor, igual, mayor que la segunda
 * respectivamente
 */
int registro_comparar_fechas(char *fecha1, char *fecha2);

/**
 * registro_imprimir: Registro -> FILE*
 * imprime el contenido de un registro dado
 * en un archivo dado
 */
void registro_imprimir(Registro reg, FILE * archivo);

/**
 * registro_restar_fechas: char* -> char* -> int
 * Dadas dos cadenas con el formato de fecha utilizado
 * resta las fechas devolviendo la diferencia de dias
 * entre estas dos (fecha1 - fecha2)
 */
int registro_restar_fechas(char *fecha1, char *fecha2);

#endif
