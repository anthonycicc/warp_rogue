#
# MacOS X make file
#


GAME_NAME ?= Warp Rogue
GAME_VERSION ?= 0.8.0
SRC_SDLMAIN ?= ./platform/mac/SDLMain.m
APP_TEMPLATE ?= ./platform/mac/Template.app
EXEC_NAME ?= $(GAME_NAME)
# 4 letter signature. Probably some OS9 legacy...
GAME_SIG ?= wrog
# Java package-style identifier
GAME_IDENT ?= net.sourceforge.todoom.wrogue
ICON_NAME ?= icon.icns
DATA_DIR ?= ../data
# Path to the SDL frameworks
SDL_FRAMEWORK_PATH ?= /Library/Frameworks


INFO_PLIST_PATTERN = s/$$(APP_NAME)/$(GAME_NAME)/;s/$$(EXEC_NAME) \
/$(EXEC_NAME)/;s/$$(VERSION)/$(GAME_VERSION)/;s/$$(ICON_NAME) \
/$(ICON_NAME)/;s/$$(SIGNATURE)/$(GAME_SIG)/;s/$$(IDENT)/$(GAME_IDENT)/


DMG = wrogue-$(GAME_VERSION).dmg
DMG_TMP = $(DMG:.dmg=.tmp.dmg)


include wrogue_sources.inc

SRC_PLATFORM = \
  platform/sdl/platform_sdl.c \
  platform/mac/platform_mac.c

SRC = \
  $(SRC_LIB) \
  $(SRC_PLATFORM) \
  $(SRC_MAIN) 
 
OBJ = $(SRC:.c=.o) $(SRC_SDLMAIN:.m=.o)
EXE = ./wrogue

CC = gcc
CFLAGS ?= -std=c99 -Wall -Os -fomit-frame-pointer -ffast-math
MFLAGS ?= -Wall -Os -fomit-frame-pointer -ffast-math

CFLAGS += -std=c99 -F"$(SDL_FRAMEWORK_PATH)" \
-I"$(SDL_FRAMEWORK_PATH)/SDL2.framework/Headers" -I. -I./lib \
-isysroot/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk -Os

MFLAGS += -I"$(SDL_FRAMEWORK_PATH)/SDL2.framework/Headers" \
-isysroot/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk -Os

LDFLAGS += -F"$(SDL_FRAMEWORK_PATH)" -framework SDL2 -framework Cocoa \
-lm -Wl,-syslibroot,/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk

RM = rm -rf

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.m
	$(CC) $(MFLAGS) -o $@ -c $<

$(EXE): $(OBJ) $(RES)
	$(CC) $(OBJ) $(RES) $(LDFLAGS) -o $@

.PHONY : all	
all: $(EXE)
	strip $(EXE)

.PHONY : dist
dist: $(DMG)

$(DMG): app
	-$(RM) "$(DMG)"
	hdiutil create -srcfolder "app" -fs HFS+ -volname "$(GAME_NAME)" $(DMG_TMP)
	hdiutil convert -format UDZO -imagekey zlib-level=9 $(DMG_TMP) -o $(DMG)
	hdiutil internet-enable -yes $(DMG)
	-$(RM) $(DMG_TMP)

.PHONY : app
app: $(EXE)
	-$(RM) "app"
	mkdir app
	cp -R "$(APP_TEMPLATE)" "app/$(GAME_NAME).app"
	sed '$(INFO_PLIST_PATTERN)' "$(APP_TEMPLATE)/Contents/Info.plist" > "app/$(GAME_NAME).app/Contents/Info.plist"
	cp $(EXE) "app/$(GAME_NAME).app/Contents/MacOS/$(EXEC_NAME)"
	ln -s "Contents/MacOS/$(EXEC_NAME)" "app/$(GAME_NAME).app/$(EXEC_NAME)"
	cp -R "$(SDL_FRAMEWORK_PATH)/SDL2.framework" "app/$(GAME_NAME).app/Contents/Frameworks"
	rm -r "app/$(GAME_NAME).app/Contents/Frameworks/SDL2.framework/Versions/A/Headers"
	rm "app/$(GAME_NAME).app/Contents/Frameworks/SDL2.framework/Headers"
	cp -R "$(DATA_DIR)/"* "app/$(GAME_NAME).app/Contents/Resources"	
	cp "./platform/mac/$(ICON_NAME)" "app/$(GAME_NAME).app/Contents/Resources/"

.PHONY : release
release: app
	rm -rf "../$(GAME_NAME).app"
	mv -f "./app/$(GAME_NAME).app" ..

.PHONY : clean
clean:
	-$(RM) $(OBJ)
	-$(RM) $(EXE) $(DMG) "$(GAME_NAME).app"


