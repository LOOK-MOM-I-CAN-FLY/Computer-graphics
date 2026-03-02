CXX := g++
SRC := main.cpp        
TARGET := daynight


PKG_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs glfw3 2>/dev/null)


CXXFLAGS := -std=c++11 -O2 -Wall $(PKG_CFLAGS)
LDFLAGS  := $(PKG_LIBS) -lGL -lm


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
