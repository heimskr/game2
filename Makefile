ifeq ($(BUILD),release)
BUILDFLAGS := -O3
else
BUILDFLAGS := -g -O0
endif

ifeq ($(OS),Windows_NT)
DEPS       := gtk4 gtkmm-4.0
OUTPUT     := game.exe
else
DEPS       := gtk4 gtkmm-4.0 x11
OUTPUT     := game
endif

CPP        := g++
CPPFLAGS   := -Wall -Wextra $(BUILDFLAGS) -std=c++20 -Iinclude
INCLUDES   := $(shell pkg-config --cflags $(DEPS))
LIBS       := $(shell pkg-config --libs   $(DEPS))
LDFLAGS    := -pthread
SOURCES    := $(shell find src -name \*.cpp) src/resources.cpp
OBJECTS    := $(SOURCES:.cpp=.o)
GLIB_COMPILE_RESOURCES = $(shell pkg-config --variable=glib_compile_resources gio-2.0)

.PHONY: all

all: $(OUTPUT)

src/resources.cpp: game2.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=resources --generate-dependencies game2.gresource.xml)
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=resources --generate-source $<

%.o: %.cpp
	@ printf "\e[2m[\e[22;32mCC\e[39;2m]\e[22m $< \e[2m$(BUILDFLAGS)\e[22m\n"
	@ $(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	@ printf "\e[2m[\e[22;36mLD\e[39;2m]\e[22m $@\n"
	@ $(CPP) $^ -o $@ $(LIBS) $(LDFLAGS)
ifeq ($(BUILD),release)
	strip $@
endif

test: $(OUTPUT)
	./$(OUTPUT)

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	@ echo rm -f $$\(OBJECTS\) $(OUTPUT) src/resources.cpp
	@ rm -f $(OBJECTS) $(OUTPUT)

install: $(OUTPUT)
	mkdir -p ~/.cache/gnome-builder/install/game2/host/
	cp $(OUTPUT) ~/.cache/gnome-builder/install/game2/host/

count:
	cloc . --exclude-dir=.vscode

countbf:
	cloc --by-file . --exclude-dir=.vscode

install-desktop-file:
	@ mkdir -p ~/.local/share/applications
	sed -e "s#@bindir@#$$PWD#" desktop.in > ~/.local/share/applications/game2.desktop

DEPFILE  := .dep
DEPTOKEN := "\# MAKEDEPENDS"

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend -f $(DEPFILE) -s $(DEPTOKEN) -- $(CPP) $(CPPFLAGS) -- $(SOURCES) 2>/dev/null
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
