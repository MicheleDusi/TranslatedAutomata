# Nome del file eseguibile di output
OUTPUT = translated_automata

# Directories
SRCDIR = ./src
BINDIR = ./bin
INCDIR = ./include
OBJDIR = ./obj

# Parametri di compilazione
CC = g++
CFLAGS=-I$(INCDIR) #-std=c++11

# Nomi (base) dei file da compilare
SOURCES := $(shell find $(SRCDIR) -name '*.cpp')					# Sources ".cpp"
HEADERS := $(shell find $(INCDIR) -name '*.hpp')					# Headers ".hpp"
OBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))	# Objects ".o"

# Compilazione dei sorgenti nei singoli file obj
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -g -c -o $@ $<

# Compilazione e linking nell'eseguibile finale
$(BINDIR)/$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) -g -o $@ $^


# Pulizia dei file
# 	Usage: "make clean"
.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/*
