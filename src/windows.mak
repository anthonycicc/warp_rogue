#
# MinGW (pure MinGW i.e. no MSYS) make file 
#

#
# include common source files
#
include wrogue_sources.inc


#
# platform specific source files
#
SRC_PLATFORM = \
  platform\sdl\platform_sdl.c \
  platform\windows\platform_win.c


#
# common compiler/linker flags
#
CFLAGS = -I. -I.\lib
LDFLAGS = -lmingw32 -lSDLmain -lSDL -mwindows



SRC = \
  $(SRC_LIB) \
  $(SRC_PLATFORM) \
  $(SRC_MAIN)


#
# use Windows style path separator
#
SEP:=$(strip \)


OBJ=$(SRC:.c=.o)
EXE = wrogue.exe
RES = wrogue.res

CC = gcc
WINDRES = windres
RM = del



#
# target: release 
#
.PHONY: release
release: CFLAGS += -std=c99 -Wall -Os -fomit-frame-pointer -ffast-math
release: LDFLAGS += -Wl,-O1
release: STRIP_BINARY = yes
release: build



#
# target: debug
#
.PHONY: debug
debug: CFLAGS += -std=c89 -Wall -Wextra -ansi -pedantic -Wpointer-arith \
-Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement \
-Wshadow -Wmissing-declarations -Wold-style-definition -Wredundant-decls \
-g -DDEBUG
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
	@copy /Y $(EXE) .. > nul
	@copy /Y .\platform\sdl\sdl.txt .. > nul
	@echo Install complete!



#
# target: clean 
#
.PHONY : clean
clean:
	-$(RM) $(OBJ)
	-$(RM) $(RES)
	-$(RM) $(EXE)



%.o: %.c
	@echo Building $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(RES) : .\platform\windows\wrogue.rc
	@$(WINDRES) $< -O coff -o $@

$(EXE): $(OBJ) $(RES)
	@$(CC) $(OBJ) $(RES) $(LDFLAGS) -o $@

