#
# GNU/UNIX make file 
#

#
# include common source files
#
include wrogue_sources.inc


#
# platform specific source files
#
SRC_PLATFORM=\
  platform/sdl/platform_sdl.c \
  platform/unix/platform_unix.c


#
# common compiler/linker flags
#
CFLAGS=-I. -I./lib `sdl-config --cflags`
LDFLAGS=-lm `sdl-config --libs`


SRC=\
  $(SRC_LIB) \
  $(SRC_PLATFORM) \
  $(SRC_MAIN) 

OBJ=$(SRC:.c=.o)
EXE=./wrogue

CC=gcc
RM=rm -f



#
# target: release 
#
.PHONY: release
release: CFLAGS += -pipe -std=c99 -Wall -Os -fomit-frame-pointer -ffast-math
release: LDFLAGS += -Wl,-O1
release: STRIP_BINARY = yes
release: build



#
# target: debug
#
.PHONY: debug
debug: CFLAGS += -pipe -Wall -Wextra -ansi -pedantic -Wpointer-arith \
-Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement \
-Wshadow -Wmissing-declarations -Wold-style-definition -Wredundant-decls \
-ggdb -DDEBUG
debug: STRIP_BINARY = no
debug: build



#
# target: build
#
.PHONY : build
build: $(EXE)
	
	@echo Build complete!

ifeq ($(STRIP_BINARY),yes)
	@echo Stripping binary..
	@strip $(EXE)
endif

	@echo Copying files..
	@cp $(EXE) ..
	@cp ./platform/sdl/sdl.txt ..
	@cp ./platform/unix/readme.txt ..
	@cp ./platform/unix/runwrogue.sh ..
	@echo Install complete!



#
# target: clean
#
.PHONY : clean
clean:
	-$(RM) $(OBJ)
	-$(RM) $(EXE)



%.o: %.c
	@echo Building $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(EXE): $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $@



