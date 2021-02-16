
# Makefile #


CXX=g++
DEPENDENCIES=main.o wave.o riff.o
EXE=main

# Windows #
# CXXFLAGS=--std=c++11 -Wall -lws2_32
# UNIX #
# CXXFLAGS=--std=c++11 -Wall

all: clean ${EXE}

${EXE}: ${DEPENDENCIES}
	${CXX} -o $@ $^ ${CXXFLAGS}

clean:
	rm -f *.o ${EXE}
