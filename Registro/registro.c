#include "registro.h"

Registro registro_crear(char *fecha, char *dpto, char *localidad, int confirm,
                        int descart, int estudio, int notificaciones) {
  Celda *reg = malloc(sizeof(Celda));
  reg->fecha = malloc(sizeof(char) * (strlen(fecha) + 1));
  strcpy(reg->fecha, fecha);
  reg->departamento = malloc(sizeof(char) * (strlen(dpto) + 1));
  strcpy(reg->departamento, dpto);
  reg->localidad = malloc(sizeof(char) * (strlen(localidad) + 1));
  strcpy(reg->localidad, localidad);
  reg->confirmados = confirm;
  reg->descartados = descart;
  reg->enEstudio = estudio;
  reg->notificaciones = notificaciones;

  return reg;
}

int registro_is_number_no_neg(char *dato) {
  int alpha = 0;
  for (int i = 0; dato[i] && !alpha; i++) {
    if (!isdigit(dato[i]) && dato[i] != '\n')
      alpha = 1;
  }
  return !alpha;

}

int registro_fecha_valida(char *fecha) {
  int anio, mes, dia, nchar = -1;
  if (sscanf(fecha, "%d-%d-%d%n", &anio, &mes, &dia, &nchar) == 3) {
    return anio >= 2019 && mes >= 1 && mes <= 12 && dia <= 31 && dia >= 1
        && !strcmp(fecha + nchar, "T00:00:00-03:00");
  }
  return 0;
}

Registro registro_crear_from_string(char *entrada, int incluyeNotificaciones) {
  char *fecha, *departamento, *localidad, *confirm, *descart, *estudio;
  int confirmados, descartados, enEstudio, notificaciones;
  if (!(fecha = strtok(entrada, ",")))
    return NULL;
  if (!registro_fecha_valida(fecha))
    return NULL;
  if (!(departamento = strtok(NULL, ",")))
    return NULL;
  if (!(localidad = strtok(NULL, ",")))
    return NULL;
  if (!(confirm = strtok(NULL, ",")))
    return NULL;
  if (!registro_is_number_no_neg(confirm))
    return NULL;
  confirmados = atoi(confirm);
  if (!(descart = strtok(NULL, ",")))
    return NULL;
  if (!registro_is_number_no_neg(descart))
    return NULL;
  descartados = atoi(descart);
  if (!(estudio = strtok(NULL, ",")))
    return NULL;
  if (!registro_is_number_no_neg(estudio))
    return NULL;

  enEstudio = atoi(estudio);
  if (incluyeNotificaciones) {
    char *noti;
    if (!(noti = strtok(NULL, ",")))
      return NULL;
    if (!registro_is_number_no_neg(noti))
      return NULL;
    notificaciones = atoi(noti);
  } else
    notificaciones = confirmados + descartados + enEstudio;

  if (strtok(NULL, ""))
    return NULL;

  return registro_crear(fecha, departamento, localidad, confirmados,
                        descartados, enEstudio, notificaciones);
}

void registro_destruir(Registro reg) {
  free(reg->fecha);
  free(reg->departamento);
  free(reg->localidad);
  free(reg);
}

unsigned long registro_hash_fdl(char *dpto, char *localidad) {
  char combi[strlen(dpto) + 1];
  char copia[strlen(localidad) + 1];
  strcpy(copia, localidad);
  strcpy(combi, dpto);
  strcat(combi, copia);
  return registro_string_clave_hash(combi);
}

unsigned long registro_string_clave_hash(void *dato) {
  unsigned long hash = 0;
  char *loc = (char *) dato;
  for (int i = 0; loc[i]; i++)
    hash = loc[i] + (hash << 6) + (hash << 16) - hash;
  return hash;
}

unsigned long registro_clave_hash(void *dato) {
  Registro reg = (Registro) dato;
  char combinado[strlen(reg->departamento) + 1];
  strcpy(combinado, reg->departamento);
  char copia[strlen(reg->localidad) + 1];
  strcpy(copia, reg->localidad);
  strcat(combinado, copia);
  return registro_string_clave_hash(combinado);
}

int registro_comparar(Registro reg1, Registro reg2) {
  if (!strcmp(reg1->departamento, reg2->departamento)
      && !strcmp(reg1->localidad, reg2->localidad)
      && !strcmp(reg1->fecha, reg2->fecha))
    return 0;
  return 1;
}

int registro_comparar_fld(Registro reg, char *fecha, char *loc, char *dpto) {
  if (!strcmp(reg->localidad, loc) && !strcmp(reg->fecha, fecha)
      && !strcmp(reg->departamento, dpto))
    return 0;
  return 1;
}

int registro_comparar_ld(Registro reg, char *loc, char *dpto) {
  if (!strcmp(reg->localidad, loc) && !strcmp(reg->departamento, dpto))
    return 0;
  return 1;
}

int registro_comparar_fechas(char *fecha1, char *fecha2) {
  // "2020-11-22T00:00:00-03:00"
  int anio1, mes1, dia1, anio2, mes2, dia2;
  sscanf(fecha1, "%d-%d-%dT", &anio1, &mes1, &dia1);
  sscanf(fecha2, "%d-%d-%dT", &anio2, &mes2, &dia2);
  if (anio1 > anio2)
    return 1;
  if (anio1 < anio2)
    return -1;
  // anio1 == anio2
  if (mes1 > mes2)
    return 1;
  if (mes1 < mes2)
    return -1;
  // mes1 == mes2
  if (dia1 > dia2)
    return 1;
  if (dia1 < dia2)
    return -1;
  // dia1 == dia2
  return 0;
}

void registro_imprimir(Registro reg, FILE * archivo) {
  fprintf(archivo, "%s,%s,%s,%d,%d,%d,%d\n", reg->fecha, reg->departamento,
          reg->localidad, reg->confirmados, reg->descartados, reg->enEstudio,
          reg->notificaciones);
}

int registro_restar_fechas(char *fecha1, char *fecha2) {
  int anio1, mes1, dia1, anio2, mes2, dia2;
  sscanf(fecha1, "%d-%d-%dT", &anio1, &mes1, &dia1);
  sscanf(fecha2, "%d-%d-%dT", &anio2, &mes2, &dia2);
  struct tm tiempo1 = { 0 }, tiempo2 = { 0 };

  tiempo1.tm_year = anio1 - 1900;
  tiempo1.tm_mon = mes1 - 1;
  tiempo1.tm_mday = dia1;

  tiempo2.tm_year = anio2 - 1900;
  tiempo2.tm_mon = mes2 - 1;
  tiempo2.tm_mday = dia2;

  double diferencia = difftime(mktime(&tiempo1), mktime(&tiempo2));
  return (int) (diferencia / (60 * 60 * 24));
}
