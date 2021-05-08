#include "rlist.h"

RList rlist_inicializar() {
  RList lista = malloc(sizeof(Extremos));
  lista->primero = NULL;
  lista->ultimo = NULL;
  lista->cantidad = 0;
  lista->ordenada = 0;
  return lista;
}

void rlist_imprimir(RList lista, FILE * archivo) {
  if (lista) {
    RNodo *nodoAux = lista->primero;
    for (; nodoAux != NULL; nodoAux = nodoAux->sig) {
      registro_imprimir(nodoAux->reg, archivo);
    }
  }
}

RList rlist_agregar_final(RList lista, Registro reg) {
  RNodo *nuevoNodo = malloc(sizeof(RNodo));
  nuevoNodo->reg = reg;
  nuevoNodo->sig = NULL;
  nuevoNodo->ant = lista->ultimo;
  if (lista->ultimo != NULL)
    lista->ultimo->sig = nuevoNodo;
  if (lista->primero == NULL)
    lista->primero = nuevoNodo;
  lista->ultimo = nuevoNodo;
  lista->cantidad++;
  lista->ordenada = 0;
  return lista;
}

RList rlist_agregar_final_reemplazar(RList lista, Registro reg) {
  RNodo *temp = lista->primero;
  int reemplazo = 0;
  for (; temp != NULL && !reemplazo; temp = temp->sig) {
    if (!registro_comparar(reg, temp->reg)) {
      registro_destruir(temp->reg);
      temp->reg = reg;
      reemplazo = 1;
      lista->ordenada = 0;
    }
  }
  if (!reemplazo)
    return rlist_agregar_final(lista, reg);

  return lista;
}

void rlist_destruir(RList lista) {
  RNodo *nodoAux = lista->primero;
  while (nodoAux != NULL) {
    registro_destruir(nodoAux->reg);
    lista->primero = nodoAux->sig;
    free(nodoAux);
    nodoAux = lista->primero;
  }
  free(lista);
}

void rlist_eliminar_nodo(RList lista, RNodo * nodoAEliminar) {
  if (lista->primero == lista->ultimo) {
    lista->primero = NULL;
    lista->ultimo = NULL;
    lista->cantidad = 0;
  } else if (nodoAEliminar->ant == NULL) {
    (nodoAEliminar->sig)->ant = NULL;
    lista->primero = nodoAEliminar->sig;
  } else if (nodoAEliminar->sig == NULL) {
    (nodoAEliminar->ant)->sig = NULL;
    lista->ultimo = nodoAEliminar->ant;
  } else {
    (nodoAEliminar->sig)->ant = nodoAEliminar->ant;
    (nodoAEliminar->ant)->sig = nodoAEliminar->sig;
  }
  lista->cantidad--;
  registro_destruir(nodoAEliminar->reg);
  free(nodoAEliminar);
}

RList rlist_eliminar_by_fld(RList lista, char *fecha, char *localidad,
                            char *dpto) {
  RNodo *temp = lista->primero;
  int eliminado = 0;
  while (temp != NULL && !eliminado) {
    if (!registro_comparar_fld(temp->reg, fecha, localidad, dpto)) {
      rlist_eliminar_nodo(lista, temp);
      eliminado = 1;
    }
    if (!eliminado)
      temp = temp->sig;
  }
  if (!eliminado)
    printf("No se encontro registro a eliminar\n");
  return lista;

}

int rlist_buscar_pico(RList lista, char *dpto, char *localidad) {
  if (lista && !lista->ordenada)
    lista = rlist_mergeSort(lista);
  RNodo *temp = lista->primero;
  int pico = -1, aRestar = 0;
  for (; temp != NULL; temp = temp->sig) {
    if (!registro_comparar_ld(temp->reg, localidad, dpto)) {
      if (temp->reg->confirmados - aRestar > pico)
        pico = temp->reg->confirmados - aRestar;
      aRestar = temp->reg->confirmados;
    }
  }

  return pico;
}

int rlist_buscar_acumulado(RList lista, char *fecha, char *dpto,
                           char *localidad) {
  RNodo *temp = lista->primero;
  int acumulados = -1, termino = 0;
  for (; temp != NULL && !termino; temp = temp->sig) {
    if (!registro_comparar_ld(temp->reg, localidad, dpto)) {
      int comp = registro_comparar_fechas(temp->reg->fecha, fecha);
      if (!comp) {
        acumulados = temp->reg->confirmados;
        termino = 1;
      } else if (comp < 0) {
        if (temp->reg->confirmados > acumulados)
          acumulados = temp->reg->confirmados;
      }
    }
  }

  return acumulados;
}

int rlist_tiempo_duplicacion(RList lista, char *fecha, char *dpto,
                             char *localidad) {
  RNodo *temp = lista->primero;
  Registro buscado = NULL;
  int tiempoDuplicacion = -1, termino = 0;
  for (; temp != NULL && !termino; temp = temp->sig) {
    if (!registro_comparar_fld(temp->reg, fecha, localidad, dpto)) {
      termino = 1;
      buscado = temp->reg;
    }
  }
  if (buscado) {
    temp = lista->primero;
    termino = 0;
    int mitad = (buscado->confirmados) / 2;
    int margenError = (float) mitad * 0.05 == 0 ? 1 : mitad * 0.05;
    Registro ultimo = NULL;
    for (; temp != NULL && !termino; temp = temp->sig) {
      if (!registro_comparar_ld(temp->reg, localidad, dpto)) {
        int comp = registro_comparar_fechas(temp->reg->fecha, fecha);
        if (comp) {
          if (temp->reg->confirmados == mitad) {
            ultimo = temp->reg;
            termino = 1;
          } else if (temp->reg->confirmados >= (mitad - margenError)
                     && temp->reg->confirmados <= (mitad + margenError)) {
            if (!ultimo
                || abs(ultimo->confirmados - mitad) >
                abs(temp->reg->confirmados - mitad)) {
              ultimo = temp->reg;
            }
          }
        }
      }
    }
    if (ultimo)
      tiempoDuplicacion = registro_restar_fechas(buscado->fecha, ultimo->fecha);
  }

  return tiempoDuplicacion;
}

void rlist_graficar(RList lista, char *fechaInicio, char *fechaFin, char *dpto,
                    char *localidad) {
  if (lista && !lista->ordenada)
    lista = rlist_mergeSort(lista);
  RNodo *temp = lista->primero;
  Registro anterior = NULL;
  int tamano = registro_restar_fechas(fechaFin, fechaInicio), i = 0, aRestar =
      0, dias;
  double *x = calloc(tamano + 1, sizeof(double)), 
         *y = calloc(tamano + 1, sizeof(double)),
         *y2 = calloc(tamano + 1, sizeof(double));
  for (; temp != NULL; temp = temp->sig) {
    if (!registro_comparar_ld(temp->reg, localidad, dpto)) {
      if (registro_comparar_fechas(temp->reg->fecha, fechaInicio) >= 0
          && registro_comparar_fechas(temp->reg->fecha, fechaFin) <= 0) {
        if (anterior) {
          dias = registro_restar_fechas(temp->reg->fecha, anterior->fecha);
          for (int j = 1; j < dias; j++) {
            x[i] = i + 1;
            y[i] = 0.0;
            y2[i] = 0.0;
            i++;
          }
        }
        y[i] = temp->reg->confirmados;
        x[i] = i + 1;
        y2[i] = temp->reg->confirmados - aRestar;
        i++;
        anterior = temp->reg;
      }
      aRestar = temp->reg->confirmados;
    }
  }
  dias = registro_restar_fechas(fechaFin, anterior->fecha);
  for (int j = 1; j < dias; j++) {
    x[i] = i + 1;
    y[i] = 0.0;
    y2[i] = 0.0;
    i++;
  }
  if (i) {
    gnuplot_ctrl *h1, *h2;
    h1 = gnuplot_init();
    h2 = gnuplot_init();
    gnuplot_setstyle(h1, "impulses");
    gnuplot_set_ylabel(h1, "casos");
    gnuplot_set_xlabel(h1, "dias");
    gnuplot_setstyle(h2, "impulses");
    gnuplot_set_ylabel(h2, "casos");
    gnuplot_set_xlabel(h2, "dias");
    gnuplot_plot_xy(h1, x, y, i, "casos acumulados");
    gnuplot_plot_xy(h2, x, y2, i, "casos diarios");
    gnuplot_close(h1);
    gnuplot_close(h2);
  } else {
    printf("No hay nada para graficar\n");
  }
  free(x);
  free(y);
  free(y2);
}

RList rlist_append(RNodo * nodo, RList lista) {
  nodo->ant = NULL;
  nodo->sig = lista->primero;
  lista->primero->ant = nodo;
  lista->primero = nodo;
  lista->cantidad++;

  return lista;
}

RList rlist_merge(RList lista1, RList lista2) {
  if (lista1->cantidad == 0)
    return lista2;
  if (lista2->cantidad == 0)
    return lista1;
  if (registro_comparar_fechas
      (lista1->primero->reg->fecha, lista2->primero->reg->fecha) <= 0) {
    if (lista1->cantidad == 1) {
      lista1 = rlist_append(lista1->primero, lista2);
    } else {
      RNodo *nodoAux = lista1->primero;
      lista1->primero = lista1->primero->sig;
      lista1->cantidad--;
      lista1 = rlist_merge(lista1, lista2);
      lista1 = rlist_append(nodoAux, lista1);

    }

    return lista1;
  } else {
    if (lista2->cantidad == 1) {
      lista2 = rlist_append(lista2->primero, lista1);
    } else {
      RNodo *nodoAux = lista2->primero;
      lista2->primero = lista2->primero->sig;
      lista2->cantidad--;
      lista2 = rlist_merge(lista1, lista2);
      lista2 = rlist_append(nodoAux, lista2);
    }

    return lista2;
  }
}

RList rlist_split(RList lista) {
  int medio = lista->cantidad / 2, indice = 0;
  RNodo *nodoAux = lista->primero;
  RList lista2 = malloc(sizeof(Extremos));
  for (; indice < medio; indice++, nodoAux = nodoAux->sig);
  lista2->primero = nodoAux;
  lista2->ultimo = lista->ultimo;
  lista->ultimo = nodoAux->ant;
  lista2->cantidad = lista->cantidad - medio;
  lista->cantidad = medio;

  lista2->primero->ant = NULL;
  lista->ultimo->sig = NULL;

  return lista2;
}

RList rlist_mergeSort(RList lista) {
  if (lista->cantidad <= 1)
    return lista;
  RList aux = lista;
  RList derecha = rlist_split(lista);
  derecha = rlist_mergeSort(derecha);
  lista = rlist_mergeSort(lista);

  lista = rlist_merge(lista, derecha);
  aux->primero = lista->primero;
  aux->ultimo = lista->ultimo;
  aux->cantidad = lista->cantidad;
  aux->ordenada = 1;
  lista = aux;

  free(derecha);

  return lista;
}
