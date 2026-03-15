CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -DGL_SILENCE_DEPRECATION

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    GL_LIBS = -framework OpenGL -framework GLUT
else
    GL_LIBS = -lGL -lGLU -lglut
endif

all: lab3

lab3: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o lab3 $(GL_LIBS)

run: lab3
	./lab3

clean:
	rm -f lab3
