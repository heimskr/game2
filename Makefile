CPP      := g++
CPPFLAGS := -Wall -Wextra -g -O0 -std=c++20 -Iinclude -rdynamic
INCLUDES := $(shell pkg-config --cflags gtk+-3.0) $(shell pkg-config --cflags gtkmm-3.0)
LIBS     := $(shell pkg-config --libs   gtk+-3.0) $(shell pkg-config --libs   gtkmm-3.0)
LDFLAGS  := -rdynamic -pthread
OUTPUT   := game
SOURCES  := $(shell find src -name \*.cpp)
OBJECTS  := $(SOURCES:.cpp=.o)

.PHONY: all

all: $(OUTPUT)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	$(CPP) $^ -o $@ $(LIBS) $(LDFLAGS)

test: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f *.o **/*.o $(OUTPUT)

DEPFILE  := .dep
DEPTOKEN := "\# MAKEDEPENDS"

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend -f $(DEPFILE) -s $(DEPTOKEN) -- $(CPP) $(CPPFLAGS) -- $(SOURCES) 2>/dev/null
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
