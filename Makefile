# Sursa bibliotecii
SRC = main.c

# Numele bibliotecii dinamice
OUTPUT = libso_stdio.so

# Opțiuni de compilare
CFLAGS = -fPIC -Wall

all: build

build:
	gcc $(CFLAGS) -shared -o $(OUTPUT) $(SRC)

clean:
	rm -f $(OUTPUT)

