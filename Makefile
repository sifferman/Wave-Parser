
# Makefile #


# [target]: [Dependencies]
#	[commands {separated by tab}]

CXX=g++
DEPENDENCIES=main.o wave.o
EXE=main
CXXFLAGS=--std=c++11 -Wall -lws2_32
# CXXFLAGS=--std=c++11 -Wall
# REMOVE=

all: clean ${EXE}

${EXE}: ${DEPENDENCIES}
	${CXX} -o $@ $^ ${CXXFLAGS}

clean:
	rm -f *.o ${EXE}
# ${REMOVE}
