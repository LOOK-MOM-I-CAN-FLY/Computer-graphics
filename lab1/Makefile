# Makefile (macOS) — clang++ + GLFW (Homebrew)
CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -DGL_SILENCE_DEPRECATION

SRC := main.cpp
OBJ := $(SRC:.cpp=.o)
BIN := daynight

# Попробуем найти где установлен glfw через brew (works on Intel & Apple Silicon)
BREW_GLFW := $(shell brew --prefix glfw 2>/dev/null)
ifeq ($(BREW_GLFW),)
  GLFW_INCDIR :=
  GLFW_LIBDIR :=
else
  GLFW_INCDIR := -I$(BREW_GLFW)/include
  GLFW_LIBDIR := -L$(BREW_GLFW)/lib
endif

# Альтернатива: использовать pkg-config (если установлен)
PKG_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs glfw3 2>/dev/null)

# Приоритет: pkg-config если вернул что-то, иначе -brew paths
ifeq ($(PKG_LIBS),)
  LINK_LIBS := $(GLFW_LIBDIR) -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
  CPPFLAGS := $(GLFW_INCDIR)
else
  LINK_LIBS := $(PKG_LIBS) -framework OpenGL
  CPPFLAGS := $(PKG_CFLAGS)
endif

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LINK_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

run: all
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all run clean
