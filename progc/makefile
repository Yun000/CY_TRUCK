OUTPUT = T

CC = gcc

CFLAGS = -Wall -Wextra -g

all: $(OUTPUT) run

$(OUTPUT): T.c
	$(CC) $(CFLAGS) -o $(OUTPUT) T.c

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f $(OUTPUT)