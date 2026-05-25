#pragma once

typedef struct {
  const char *extension;
  const char *mime_type;
} MimeMap;

const char* get_mime_type(const char *filename);
