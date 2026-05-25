CC     = gcc
CFLAGS = -Wall -Wextra -I include/
SRC    = src/main.c src/server.c src/http.c src/files.c src/mime.c
OBJ    = $(SRC:.c=.o)
BIN    = minihttpd

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
