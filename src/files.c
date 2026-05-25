#include "files.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>


FileResult serve_file(const char *uri, const char *base_path){
  FileResult result = {NULL,0,500};
  char path[PATH_MAX];
  char resolved[PATH_MAX];
  char resolved_base[PATH_MAX];

  // Resolver base_path a absoluto para poder comparar correctamente
  if(!realpath(base_path, resolved_base)){
    result.status = 500;
    return result;
  }

  snprintf(path, sizeof(path), "%s%s", resolved_base, uri);

  if(!realpath(path, resolved)){ // Resuelve el path absoluto y maneja enlaces simbólicos
    result.status = 404;
    return result;
  }

  if(strncmp(resolved, resolved_base, strlen(resolved_base)) != 0){ // Verifica que el archivo esté dentro del directorio base
    result.status = 403;
    return result;
  }

  struct stat st; // Estructura para obtener información del archivo

  if(stat(resolved, &st) < 0){ // Verifica si el archivo existe
    result.status = 404;
    return result;
  }

  if(!(st.st_mode & S_IRUSR)){ // Permisos de Lectura para el propietario atraves de umask se debe tener el permiso 00400
    result.status = 403;
    return result;
  }

  int fd = open(resolved, O_RDONLY); // Abre el archivo en modo lectura
  if(fd < 0){
    result.status = 500;
    return result;
  }

  result.data = malloc(st.st_size);
  if(!result.data){
    close(fd);
    result.status = 500;
    return result;
  } 

  ssize_t bytes_read = read(fd, result.data, st.st_size);
  close(fd);

  if(bytes_read != st.st_size){
    free(result.data);
    result.status = 500;
    return result;
  }

  result.size = st.st_size;
  result.status = 200;

  return result;
}

void free_file_result(FileResult *result){
  if(result->data){
    free(result->data);
    result->data = NULL;
  }
}
