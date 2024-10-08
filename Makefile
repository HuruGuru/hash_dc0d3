# Makefile

IDIR = include
SDIR = src
ODIR = obj

CC = gcc
CFLAGS = -I$(IDIR)

_DEPS = base64.h md5.h md5_brute.h menu.h sha256.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = base64.o hash_dc0d3.o md5.o md5_brute.o menu.o sha256.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

BIN = hash_dc0d3

all: $(BIN)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: all clean

clean:
	rm -f $(ODIR)/*.o $(SDIR)/*~ $(IDIR)/*~
	rm -f $(BIN)




