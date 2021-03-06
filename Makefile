CC=g++
CFLAGS=-g --std=c++17

ODIR=obj

_OBJ = tokenizer.o scope.o builtinfunctions.o interpreter.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	g++ -o interpreter $^ $(CFLAGS)

Debug: all

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
