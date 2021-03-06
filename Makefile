SRC1 = src/main.cpp
SRC2 = src/error_handler.cpp
SRC3 = src/CPU.cpp
SRC4 = src/Virtual_memory.cpp
SRC5 = src/ISA.cpp
SRC6 = src/init.cpp
OBJ1 = $(SRC1:.cpp=.o)
OBJ2 = $(SRC2:.cpp=.o)
OBJ3 = $(SRC3:.cpp=.o)
OBJ4 = $(SRC4:.cpp=.o)
OBJ5 = $(SRC5:.cpp=.o)
OBJ6 = $(SRC6:.cpp=.o)

EXE = Processor

INSTALL_PATH = /usr/local/bin

HFILES = include/proc_defines.h include/error_handler.h include/CPU.h include/Virtual_memory.h include/init.h include/ISA.h
CFLAGS = -O2
INCDIR = -Iinclude

TIMER = 0
TCOMP =

ifeq ($(TIMER), 1)
#Timer compilation is ON
TCOMP = -DTIME=1
endif

all : $(EXE)

$(EXE) : $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)
	g++ -o $(EXE) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)

$(OBJ1) : $(SRC1) $(HFILES)
	g++ $(INCDIR) -c $(CFLAGS) $(TCOMP) $(SRC1) -o $(OBJ1)

$(OBJ2) : $(SRC2) $(HFILES)
	g++ $(INCDIR) -c $(CFLAGS) $(TCOMP) $(SRC2) -o $(OBJ2)

$(OBJ3) : $(SRC3) $(HFILES)
	g++ $(INCDIR) -c $(CFLAGS) $(TCOMP) $(SRC3) -o $(OBJ3)

$(OBJ4) : $(SRC4) $(HFILES)
	g++ $(INCDIR) -c $(CFLAGS) $(TCOMP) $(SRC4) -o $(OBJ4)

$(OBJ5) : $(SRC5) $(HFILES)
	g++ $(INCDIR) -c $(CFLAGS) $(TCOMP) $(SRC5) -o $(OBJ5)

$(OBJ6) : $(SRC6) $(HFILES)
	g++ $(INCDIR) -c $(CFLAGS) $(TCOMP) $(SRC6) -o $(OBJ6)

debug: CFLAGS = -g -Wall -DDEBUG=1
debug: all

cleanup:
	@echo "Deleting object files..."
	rm -f src/*.o

clean:
	$(MAKE) -B
	$(MAKE) cleanup

install : $(EXE)
	@echo "Installing executable to path" $(INSTALL_PATH)
	cp $(EXE) $(INSTALL_PATH)/$(EXE)
