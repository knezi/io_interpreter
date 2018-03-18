CC=g++
CFLAGS=-g --std=c++14

ODIR=obj

_OBJ = tokenizer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	g++ -o compiler $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
