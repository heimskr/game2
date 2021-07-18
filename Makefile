CPP      := g++
CPPFLAGS := -Wall -Wextra -g -O0 -std=c++20 -Iinclude -rdynamic
INCLUDES := $(shell pkg-config --cflags gtk4) $(shell pkg-config --cflags gtkmm-4.0)
LIBS     := $(shell pkg-config --libs   gtk4) $(shell pkg-config --libs   gtkmm-4.0)
LDFLAGS  := -rdynamic -pthread
OUTPUT   := game
SOURCES  := $(shell find src -name \*.cpp)
OBJECTS  := $(SOURCES:.cpp=.o)

.PHONY: all

all: $(OUTPUT)

%.o: %.cpp
	@ echo "\e[2m[\e[22;32mCC\e[39;2m]\e[22m $<"
	@ $(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	@ echo "\e[2m[\e[22;36mLD\e[39;2m]\e[22m $@"
	@ $(CPP) $^ -o $@ $(LIBS) $(LDFLAGS)

test: $(OUTPUT)
	./$(OUTPUT)

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f *.o **/*.o $(OUTPUT)

install: $(OUTPUT)
	mkdir -p ~/.cache/gnome-builder/install/game2/host/
	cp $(OUTPUT) ~/.cache/gnome-builder/install/game2/host/

count:
	cloc . --exclude-dir=.vscode

countbf:
	cloc --by-file . --exclude-dir=.vscode

DEPFILE  := .dep
DEPTOKEN := "\# MAKEDEPENDS"

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend -f $(DEPFILE) -s $(DEPTOKEN) -- $(CPP) $(CPPFLAGS) -- $(SOURCES) 2>/dev/null
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
