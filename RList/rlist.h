#ifndef __RLIST_H__
#define __RLIST_H__

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Registro/registro.h"
#include "../gnuplot_i/gnuplot_i.h"

typedef struct _RNodo {
  Registro reg;
  struct _RNodo *sig;
  struct _RNodo *ant;
} RNodo;

typedef struct {
  RNodo *primero;
  RNodo *ultimo;
  int cantidad;
  unsigned ordenada:1;
} Extremos;

typedef Extremos *RList;

/**
 * RList_inicializar: -> RList
 * Esta funcion se encarga de darle espacio a una rlist
 * e inicializar sus valores
 */
RList rlist_inicializar();

/**
 * rlist_imprimir: RList -> FILE*
 * Esta funcion imprime todos los registros de una lista
 */
void rlist_imprimir(RList lista, FILE * archivo);

/**
 * rlist_agregar_final: RList -> Registro -> RList
 * Esta funcion agrega un registro al final de ella
 */
RList rlist_agregar_final(RList lista, Registro reg);

/**
 * rlist_agregar_final_reemplazar: RList -> Registro
 * Esta funcion agrega un registro al final de ella
 * o lo reemplaza en caso de ya existir
 */
RList rlist_agregar_final_reemplazar(RList lista, Registro reg);

/**
 * rlist_destruir: RList 
 * Esta funcion elimina la lista liberando los punteros
 * involucrados
 */
void rlist_destruir(RList lista);

/**
 * rlist_destruir: RList -> RNodo
 * Esta funcion toma una lista y un nodo
 * y elimina el nodo de la lista.
 */
void rlist_eliminar_nodo(RList lista, RNodo * nodoAEliminar);

/**
 * rlist_eliminar_by_fld: RList -> char* -> char* -> char* -> RList
 * Esta funcion busca un Registro en una lista dada segun la fecha,
 * localidad y departamento dados. Si lo encuentra, lo elimina.
 */
RList rlist_eliminar_by_fld(RList lista, char *fecha, char *localidad,
                            char *dpto);

/**
 * rlist_buscar_pico: RList -> char* -> char* -> RList
 * Busca el pico de casos confirmados diarios en una lista de Registros dada
 * segun un departamento y localidad dada
 */
int rlist_buscar_pico(RList lista, char *dpto, char *localidad);

/**
 * rlist_buscar_acumulado: RList -> char* -> char* -> char* -> int
 * Busca la cantidad de casos acumulados maxima en los registros
 * de una lista dada como argumento que tengan el departamento y localidad dados
 * y la fecha no supere a la fecha dada como argumento.
 */
int rlist_buscar_acumulado(RList lista, char *fecha, char *dpto,
                           char *localidad);

/**
 * rlist_tiempo_duplicacion: RList -> char* -> char* -> char* -> int
 * Estima el tiempo de duplicacion de casos confirmados acumulados
 * en los registros de una lista dada como argumento 
 * que tengan el departamento y localidad dada (la fecha debe existir)
 * la estimacion sera el resultado exacto cuando se encuentre la mitad
 * exacta de casos acumulados de la fecha dada en otro registro de mismo dpto y localidad
 */
int rlist_tiempo_duplicacion(RList lista, char *fecha, char *dpto,
                             char *localidad);


/**
 * rlist_graficar: RList -> char* -> char* -> char* -> char*
 * Dada una lista, una fecha de inicio, una fecha de fin, un departamento
 * y una localidad grafica los casos acumulados y diarios de los registros
 * de la lista que pertenezcan a ese departamento y localidad y se encuentren
 * cargados entre la fecha de inicio y la de fin.
 */
void rlist_graficar(RList lista, char *fechaInicio, char *fechaFin, char *dpto,
                    char *localidad);

/**
 * rlist_append: RNodo* -> RList -> RList
 * Esta funcion toma un nodo ya existente y una lista, 
 * y agrega el nodo al principio de la lista.
 */
RList rlist_append(RNodo * nodo, RList lista);

/**
 * rlist_merge: RList -> RList -> RList
 * Esta funcion toma dos listas y devuelve una 
 * lista con los elementos de ambas, 
 * ordenados segun la fecha de menor a mayor.
 */
RList rlist_merge(RList lista1, RList lista2);

/**
 * rlist_split: RList -> RList
 * Funcion que devuelve aproximadamente la mitad derecha de la lista
 * y deja la otra lista como aproximadamente la mitad izquierda.
 */
RList rlist_split(RList lista);

/**
 * rlist_mergeSort: RList 
 * Implementacion del algoritmo merge sort para listas enlazadas
 * (no crea una lista nueva)
 */
RList rlist_mergeSort(RList lista);

#endif
