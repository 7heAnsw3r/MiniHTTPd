# MiniHTTPd

Servidor HTTP/1.1 básico implementado en C sin librerías externas.

## Librerías del sistema utilizadas

- `sys/epoll.h` — gestión de múltiples conexiones simultáneas
- `sys/socket.h` — creación y manejo de sockets TCP
- `netinet/in.h` — estructuras de direcciones de red
- `sys/stat.h` — información de archivos del sistema
- `fcntl.h` — operaciones sobre file descriptors
- `unistd.h` — llamadas al sistema (read, write, close)
- `string.h` — manejo de cadenas de caracteres
- `stdlib.h` — gestión de memoria dinámica

## Referencias

- Video: https://www.youtube.com/watch?v=OOugpd0BqT0&t=289s
- Blog: https://bruinsslot.jp/post/simple-http-webserver-in-c/
- Inteligencia artificial utilizada para la generación del `index.html` y `style.css`
