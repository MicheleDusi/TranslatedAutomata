# Nome del file eseguibile di output
OUTPUT = translated_automata

# Directories
SRCDIR = src
BINDIR = build
IDIR = include
ODIR = obj

# Parametri di compilazione
CC = g++
CFLAGS=-I$(IDIR) #-std=c++11

# Nomi (base) dei file da compilare
FILES = state state_dfa state_nfa main

# Queste due righe non servono, ma computano i nomi dei sorgenti
_SRCS = $(patsubst %, %.cpp, $(FILES))
SRCS = $(patsubst %, $(SRCDIR)/%, $(_SRCS))

# Queste due righe computano i nomi degli headers
_DEPS = $(patsubst %, %.hpp, $(FILES))
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

# Queste due righe computano i nomi dei file objects
_OBJS = $(patsubst %, %.o, $(FILES))
OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))

# Compilazione dei sorgenti nei singoli file obj
$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

# Compilazione e linking nell'eseguibile finale
$(BINDIR)/$(OUTPUT): $(OBJS)
	$(CC) -g -o $@ $^ $(CFLAGS)


# Pulizia dei file
# 	Usage: "make clean"
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
	rm -f $(BINDIR)/*
