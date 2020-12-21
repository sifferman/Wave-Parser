
# Makefile #


# [target]: [Dependencies]
#	[commands {separated by tab}]

CXX=g++
DEPENDENCIES=main.o wave.o helpers.o
EXE=main
CXXFLAGS=--std=c++11
# REMOVE=

all: clean ${EXE}

${EXE}: ${DEPENDENCIES}
	${CXX} ${CXXFLAGS} -o $@ $^

# debug: ${DEPENDENCIES}
# 	${CXX} -g -std=c++11 $^

# $@ ··· [target]
# $^ ··· {Dependencies}

clean:
	rm -f *.o ${EXE}
# ${REMOVE}
