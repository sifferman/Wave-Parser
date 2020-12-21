
/* main.cpp */


#include "wave.h"
#include "helpers.h"

#include <iostream>
#include <fstream>

using namespace std;

class CouldNotOpenFile { };

int main() {

    ifstream ifs( "TESTS/1.wav" );

    if ( !ifs )
        throw CouldNotOpenFile();

    // get length of file:
    ifs.seekg ( 0, ifs.end );
    size_t buffer_size = ifs.tellg();
    ifs.seekg ( 0, ifs.beg );

    // load into buffer
    char * buffer = new char[ buffer_size ];
    ifs.read( buffer, buffer_size );
    ifs.close();



    std::ofstream out("out.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!


    // Wave().printFmt();
    Wave w( buffer, buffer_size );
    w.printFmt();
    w.printData();



    std::cout.rdbuf(coutbuf); //reset to standard output again
    return 0;
}

/* 
    std::ofstream out("out.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt

    // do stuff

    std::cout.rdbuf(coutbuf); //reset to standard output again
 */