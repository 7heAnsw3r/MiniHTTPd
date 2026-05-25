#pragma once
#include <stddef.h>

typedef struct {
  char *data;
  size_t size;
  int status; // Estatus al obtener el archivo
} FileResult;


FileResult serve_file(const char *uri, const char *base_path);
void free_file_result(FileResult *result);

