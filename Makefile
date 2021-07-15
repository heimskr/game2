CPP      := g++
CPPFLAGS := -Wall -Wextra -g -O0 $(shell pkg-config --cflags gtk+-3.0) $(shell pkg-config --cflags gtkmm-3.0)
LDFLAGS  := $(shell pkg-config --libs gtk+-3.0) $(shell pkg-config --libs gtkmm-3.0)
OUTPUT   := game
SOURCES  := $(shell find src -name \*.cpp)
OBJECTS  := $(SOURCES:.cpp=.o)

.PHONY: all

all: $(OUTPUT)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	$(CPP) $^ -o $@ $(LDFLAGS)

test: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f *.o **/*.o $(OUTPUT)
