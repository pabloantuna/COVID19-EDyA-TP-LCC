#include "tablahash.h"

TablaHash tabla_crear(unsigned size) {
  TablaHash tabla = malloc(sizeof(Tabla));
  tabla->Tabla = malloc(sizeof(RList) * size);
  tabla->capacidad = size;
  tabla->numElems = 0;
  for (unsigned i = 0; i < tabla->capacidad; i++)
    tabla->Tabla[i] = rlist_inicializar();

  return tabla;
}

TablaHash tabla_reiniciar(TablaHash tabla) {
  unsigned size = tabla->capacidad;
  tabla_destruir(tabla);
  return tabla_crear(size);
}

TablaHash tabla_resize(TablaHash tabla, unsigned nuevoSize) {
  TablaHash tablaNueva = tabla_crear(nuevoSize);
  RList lista;
  unsigned nuevaClave;
  for (unsigned i = 0; i < tabla->capacidad; i++) {
    lista = tabla->Tabla[i];
    if (lista->primero) {
      nuevaClave =
          crear_clave(nuevoSize, lista->primero->reg, &registro_clave_hash);
      tablaNueva->Tabla[nuevaClave]->cantidad = tabla->Tabla[i]->cantidad;
      tablaNueva->Tabla[nuevaClave]->ordenada = tabla->Tabla[i]->ordenada;
      tablaNueva->Tabla[nuevaClave]->primero = tabla->Tabla[i]->primero;
      tablaNueva->Tabla[nuevaClave]->ultimo = tabla->Tabla[i]->ultimo;
      free(tabla->Tabla[i]);
      tablaNueva->numElems++;
    } else {
      free(lista);
    }
  }
  free(tabla->Tabla);
  free(tabla);

  return tablaNueva;
}

unsigned crear_clave(unsigned size, void *dato, FuncionHash fun) {
  return (unsigned) (fun(dato) % size);
}

unsigned crear_clave_fdl(unsigned size, char *dpto, char *localidad) {
  char combi[strlen(dpto) + 1];
  char copia[strlen(localidad) + 1];
  strcpy(copia, localidad);
  strcpy(combi, dpto);
  strcat(combi, copia);
  return crear_clave(size, combi, &registro_string_clave_hash);
}

void tabla_imprimir(TablaHash tabla, char *nombreArchivo, char *cabecera) {
  FILE *archivo = fopen(nombreArchivo, "w+");
  if (cabecera)
    fprintf(archivo, "%s\n", cabecera);
  for (unsigned i = 0; i < tabla->capacidad; i++) {
    rlist_imprimir(tabla->Tabla[i], archivo);
  }
  fclose(archivo);
}

void tabla_destruir(TablaHash tabla) {
  for (unsigned i = 0; i < tabla->capacidad; i++)
    rlist_destruir(tabla->Tabla[i]);
  free(tabla->Tabla);
  free(tabla);
}

TablaHash tabla_insertar_reemplazar(TablaHash tabla, Registro dato) {
  if (dato && tabla) {
    int clave = crear_clave(tabla->capacidad, dato, &registro_clave_hash);
    if (!tabla->Tabla[clave]->primero)
      tabla->numElems++;
    tabla->Tabla[clave] =
        rlist_agregar_final_reemplazar(tabla->Tabla[clave], dato);
    if (((float) tabla->numElems / (float) tabla->capacidad) > 0.9) {
      return tabla_resize(tabla, 2 * tabla->capacidad);
    }
  }

  return tabla;
}

TablaHash tabla_eliminar_by_fld(TablaHash tabla, char *fecha, char *localidad,
                                char *dpto) {
  int clave = crear_clave_fdl(tabla->capacidad, dpto, localidad);
  if (tabla->Tabla[clave]) {
    tabla->Tabla[clave] =
        rlist_eliminar_by_fld(tabla->Tabla[clave], fecha, localidad, dpto);
    if (!tabla->Tabla[clave]->primero)
      tabla->numElems--;
    if (((float) tabla->numElems / (float) tabla->capacidad) < 0.2) {
      return tabla_resize(tabla, tabla->capacidad / 2);
    }
  } else {
    printf("No se encuentra registro con los datos dados\n");
  }

  return tabla;
}

int tabla_buscar_pico(TablaHash tabla, char *dpto, char *localidad) {
  int clave = crear_clave_fdl(tabla->capacidad, dpto, localidad);
  if (tabla->Tabla[clave]) {
    return rlist_buscar_pico(tabla->Tabla[clave], dpto, localidad);
  } else
    return -2;
}

int tabla_buscar_acumulado(TablaHash tabla, char *fecha, char *dpto,
                           char *localidad) {
  int clave = crear_clave_fdl(tabla->capacidad, dpto, localidad);
  if (tabla->Tabla[clave]) {
    return rlist_buscar_acumulado(tabla->Tabla[clave], fecha, dpto, localidad);
  } else
    return -2;
}

int tabla_tiempo_duplicacion(TablaHash tabla, char *fecha, char *dpto,
                             char *localidad) {
  int clave = crear_clave_fdl(tabla->capacidad, dpto, localidad);
  if (tabla->Tabla[clave])
    return rlist_tiempo_duplicacion(tabla->Tabla[clave], fecha, dpto,
                                    localidad);
  else
    return -1;
}

void tabla_graficar(TablaHash tabla, char *fechaInicio, char *fechaFin,
                    char *dpto, char *localidad) {
  int clave = crear_clave_fdl(tabla->capacidad, dpto, localidad);
  if (tabla->Tabla[clave])
    rlist_graficar(tabla->Tabla[clave], fechaInicio, fechaFin, dpto, localidad);
  else
    printf("La localidad '%s' no se encuentra en el dataset\n", localidad);
}
