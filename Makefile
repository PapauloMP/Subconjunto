# Compiler
C = g++

# Flags
INCLUDE = ./Headers
SRC = ./CPPs
ALL_SRC = $(wildcard $(SRC)/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(subst ./CPPs,.,$(ALL_SRC)))
C_Flags = -std=c++14 -Wall -g -I $(INCLUDE) -c

all: mkdir comp

mkdir:
	mkdir bin
	mkdir obj

comp: main $(OBJ)
	$(C) -o bin/execGrupo9 obj/*

main: main.cpp
	$(C) $(C_Flags) main.cpp -o obj/main.o

PrintDot.o: $(SRC)/PrintDot.cpp $(INCLUDE)/Graph.h
	$(C) $(C_Flags) $< -o obj/$@

%.o: $(SRC)/%.cpp $(INCLUDE)/%.h
	$(C) $(C_Flags) $< -o obj/$@

clear:
	rm -rf bin obj
