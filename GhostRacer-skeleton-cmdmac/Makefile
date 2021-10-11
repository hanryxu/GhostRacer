CC = g++
FREEGLUTDIR = /usr/local/Cellar/freeglut/3.2.1_1
INCLUDES = -I$(FREEGLUTDIR)/include/GL
LIBS = -L$(FREEGLUTDIR)/lib -L/opt/X11/lib -lglut -lGL -lGLU
STD = -std=c++17
CCFLAGS = -Wno-deprecated-declarations

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean

PRODUCT = GhostRacer

all: $(PRODUCT)

%.o: %.cpp $(HEADERS)
	$(CC) -c $(STD) $(CCFLAGS) $(INCLUDES) $< -o $@

$(PRODUCT): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PRODUCT)
