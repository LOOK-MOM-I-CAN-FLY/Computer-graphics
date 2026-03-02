CXX := g++
SRC := main.cpp        
TARGET := daynight


PKG_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs glfw3 2>/dev/null)


CXXFLAGS := -std=c++11 -O2 -Wall $(PKG_CFLAGS)


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDFLAGS := $(PKG_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm
else
LDFLAGS := $(PKG_LIBS) -lGL -lm
endif


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
