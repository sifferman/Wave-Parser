
# Makefile #


# [target]: [Dependencies]
#	[commands {separated by tab}]

CXX=g++
DEPENDENCIES=main.o wave.o
EXE=main
REMOVE=*.huff

all: clean ${EXE}

${EXE}: ${DEPENDENCIES}
	${CXX} -std=c++11 -o $@ $^
# Windows: ${CXX} -std=c++11 -o $@ $^
# MAC OS:  ${CXX} -stdlib=libc++ -std=c++0x -o $@ $^

debug: ${DEPENDENCIES}
	${CXX} -g -std=c++11 $^

# $@ ··· [target]
# $^ ··· {Dependencies}

clean:
	rm -f *.o ${EXE} ${REMOVE}
