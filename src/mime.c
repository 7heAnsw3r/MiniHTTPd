#include "mime.h"
#include <string.h>

static const MimeMap mime_map[] ={
  {"html", "text/html"},
  {"css", "text/css"},
  {"js", "application/javascript"},
  {"png", "image/png"},
  {"jpg", "image/jpeg"},
  {NULL,NULL}
};

const char* get_mime_type(const char* filename){
  static const char* default_mime = "application/octet-stream"; // default MIME type
  const char* ext = strrchr(filename, '.');

  if(!ext || ext ==filename){
    return default_mime;
  }

  ext++; // avanzamos un caracter para obtener solo la extension

  for(int i=0; mime_map[i].extension != NULL; i++){
    if(strcmp(ext, mime_map[i].extension) == 0){
      return mime_map[i].mime_type;
    }
  }
  return default_mime; // si no se encuentra la extension, se devuelve el MIME por defecto
}
