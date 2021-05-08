#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Registro/registro.h"
#include "TablaHash/tablahash.h"
#define MAX_CHAR 300

char *leer_comando_devolver_datos(char comando[]) {

  int c;
  unsigned int i = 0;

  // Leo primero el comando que se que no es tan largo
  // y esta separado por un espacio de los datos
  while ((c = getchar()) != ' ' && c != '\n') {
    comando[i] = (char) c;
    i++;
  }
  comando[i] = '\0';
  if (c != '\n') {
    char *datos = malloc(sizeof(char) * MAX_CHAR);
    unsigned tamano_actual = MAX_CHAR;
    i = 0;
    while ((c = getchar()) != '\n') {
      datos[i] = (char) c;
      i++;
      if (i == tamano_actual) {
        tamano_actual = i + MAX_CHAR;
        datos = realloc(datos, tamano_actual);
      }
    }

    // if para solucionar el caso donde
    // si la entrada es algo del estilo "salir " 
    // va a resultar c = '\n' en la primer iteracion
    // no va a entrar al ciclo y va a intentar realloc
    // con i = 0 lo cual rompera el programa
    if (i == 0) {
      free(datos);
      return NULL;
    }
    datos = realloc(datos, i + 1);
    datos[i] = '\0';
    return datos;
  }
  return NULL;
}

int extension_valida(char *datos) {
  int largo = strlen(datos);
  return datos[largo - 1] == 'v' && datos[largo - 2] == 's' &&
      datos[largo - 3] == 'c' && datos[largo - 4] == '.';
}

TablaHash cargar_dataset(FILE * archivo, TablaHash tabla) {
  char line[MAX_CHAR];
  Registro reg;
  int falla = 0;
  fgets(line, MAX_CHAR, archivo);
  while (fgets(line, MAX_CHAR, archivo) && !falla) {
    reg = registro_crear_from_string(line, 1);
    if (reg)
      tabla = tabla_insertar_reemplazar(tabla, reg);
    else
      falla = 1;
  }
  if (falla) {
    printf("Hubo un error al cargar el dataset\n");
    tabla = tabla_reiniciar(tabla);
  } else {
    if (((float) tabla->numElems / (float) tabla->capacidad) > 0.9)
      return tabla_resize(tabla, 2 * tabla->capacidad);

    if (((float) tabla->numElems / (float) tabla->capacidad) < 0.2)
      return tabla_resize(tabla, tabla->capacidad / 2);
  }
  return tabla;
}

TablaHash agregar_registro(char *line, TablaHash tabla) {
  Registro reg = registro_crear_from_string(line, 0);
  if (!reg) {
    printf("Hubo un error al crear el registro\n");
    return tabla;
  }
  tabla = tabla_insertar_reemplazar(tabla, reg);
  return tabla;
}

TablaHash eliminar_registro(TablaHash tabla, char *datos) {
  char fecha[MAX_CHAR], localidad[MAX_CHAR], dpto[MAX_CHAR];
  if ((sscanf(datos, "%[^,],%[^,],%[^\n]", fecha, dpto, localidad)) == 3) {
    if (!registro_fecha_valida(fecha)) {
      printf("Error: la fecha no es el formato valido\n");
      return tabla;
    }
    tabla = tabla_eliminar_by_fld(tabla, fecha, localidad, dpto);
  } else
    printf("Error: formato de comando invalido\n");
  return tabla;
}

int buscar_pico(TablaHash tabla, char *datos) {
  char localidad[MAX_CHAR], dpto[MAX_CHAR];
  int pico = -1;
  if ((sscanf(datos, "%[^,],%[^\n]", dpto, localidad)) == 2)
    pico = tabla_buscar_pico(tabla, dpto, localidad);
  else
    return -2;
  return pico;
}

int casos_acumulados(TablaHash tabla, char *datos) {
  char fecha[MAX_CHAR], localidad[MAX_CHAR], dpto[MAX_CHAR];
  int acumulados = -1;
  if ((sscanf(datos, "%[^,],%[^,],%[^\n]", fecha, dpto, localidad)) == 3) {
    if (!registro_fecha_valida(fecha))
      return -2;
    acumulados = tabla_buscar_acumulado(tabla, fecha, dpto, localidad);
  } else
    return -2;
  return acumulados;
}

int tiempo_duplicacion(TablaHash tabla, char *datos) {
  char fecha[MAX_CHAR], localidad[MAX_CHAR], dpto[MAX_CHAR];
  int td = -1;
  if ((sscanf(datos, "%[^,],%[^,],%[^\n]", fecha, dpto, localidad)) == 3) {
    if (!registro_fecha_valida(fecha))
      return -2;
    td = tabla_tiempo_duplicacion(tabla, fecha, dpto, localidad);
  } else
    return -2;
  return td;
}

void graficar(TablaHash tabla, char *datos) {
  char fechaInicio[MAX_CHAR], fechaFin[MAX_CHAR], localidad[MAX_CHAR],
      dpto[MAX_CHAR];
  if ((sscanf
       (datos, "%[^,],%[^,],%[^,],%[^\n]", fechaInicio, fechaFin, dpto,
        localidad)) == 4) {
    if (registro_comparar_fechas(fechaInicio, fechaFin) <= 0)
      tabla_graficar(tabla, fechaInicio, fechaFin, dpto, localidad);
    else
      printf("Fecha de inicio es mayor a fecha final!\n");
  } else
    printf("Error: formato de comando invalido\n");
}

void imprimir_menu() {
  printf("Comandos:\n\n");
  printf("cargar_dataset entrada.csv\n");
  printf("imprimir_dataset salida.csv\n");
  printf
      ("agregar_registro fecha,departamento,localidad,confirmados,descartados,enEstudio\n");
  printf("eliminar_registro fecha,departamento,localidad\n");
  printf("buscar_pico departamento,localidad\n");
  printf("casos_acumulados fecha,departamento,localidad\n");
  printf("tiempo_duplicacion fecha,departamento,localidad\n");
  printf("graficar fechaInicio,fechaFinal,departamento,localidad\n");
  printf("salir\n\n");
}

int main() {
  int loop = 1;
  char comando[MAX_CHAR], *datos;
  TablaHash tabla = tabla_crear(TAMANO);
  imprimir_menu();
  while (loop) {
    printf("Ingrese comando\n");
    datos = leer_comando_devolver_datos(comando);
    if (!strcmp(comando, "salir")) {
      loop = 0;
      printf("Adios!\n");
    } else {
      if (datos) {
        if (!strcmp(comando, "cargar_dataset")) {
          if (extension_valida(datos)) {
            FILE *archivo = fopen(datos, "r");
            if (!archivo) {
              printf("No se encuentra el archivo '%s'\n", datos);
            } else {
              tabla = cargar_dataset(archivo, tabla);
              fclose(archivo);
            }
          } else {
            printf("Ingrese un archivo con extension .csv\n");
          }

        } else if (!strcmp(comando, "imprimir_dataset")) {
          if (extension_valida(datos)) {
            char cabecera[MAX_CHAR] =
                "Fecha,Departamento,Localidad,Confirmados,Descartados,En estudio,Notificaciones";
            tabla_imprimir(tabla, datos, cabecera);
          } else {
            printf("Ingrese un archivo con extension .csv\n");
          }
        } else if (!strcmp(comando, "agregar_registro")) {
          tabla = agregar_registro(datos, tabla);
        } else if (!strcmp(comando, "eliminar_registro")) {
          tabla = eliminar_registro(tabla, datos);
        } else if (!strcmp(comando, "buscar_pico")) {
          int pico = buscar_pico(tabla, datos);
          if (pico == -1)
            printf("Error: no se encontro registro\n");
          else if (pico == -2)
            printf("Error: formato de comando invalido\n");
          else
            printf("El pico de confirmados en un dia es: %d\n", pico);
        } else if (!strcmp(comando, "casos_acumulados")) {
          int casosAcumulados = casos_acumulados(tabla, datos);
          if (casosAcumulados == -1)
            printf("Error: no se encontro registro\n");
          else if (casosAcumulados == -2)
            printf("Error: formato de comando invalido\n");
          else
            printf("La cantidad de casos acumulados es: %d\n", casosAcumulados);
        } else if (!strcmp(comando, "tiempo_duplicacion")) {
          int td = tiempo_duplicacion(tabla, datos);
          if (td == -1)
            printf
                ("Error: no se puedo estimar el resultado (no existe la fecha o no hay suficientes datos para estimar)\n");
          else if (td == -2)
            printf("Error: formato de comando invalido\n");
          else
            printf("El tiempo de duplicacion estimado es: %d\n", td);
        } else if (!strcmp(comando, "graficar")) {
          graficar(tabla, datos);
        } else
          printf("El comando no es valido\n");
      } else
        printf("El comando no es valido\n");
    }
    if (datos)
      free(datos);
  }

  tabla_destruir(tabla);
  return 0;
}
