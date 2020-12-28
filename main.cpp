
/* main.cpp */


#include "wave.h"
#include "helpers.h"

#include <iostream>
#include <fstream>

using namespace std;

class CouldNotOpenFile { };

void doWave( uint8_t in, const char * const buffer, size_t & buffer_size );

int main() {

    ifstream ifs( "TESTS/test_44.1.wav" );

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

    // forward cout to out.txt
    std::ofstream out("out.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!


    doWave( 0x00, buffer, buffer_size );
    for ( uint8_t i = 0x01; i != 0x00; i++ )
        doWave( i, buffer, buffer_size );
    

    //reset to standard output again
    std::cout.rdbuf(coutbuf);

    return 0;
}

void doWave( uint8_t in, const char * const buffer, size_t & buffer_size ) {
    Wave w( buffer, buffer_size );
    w.smooth( in );
    w.toFile( "OUT/" + to_string(in) + ".wav" );
}

/* 
    std::ofstream out("out.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt

    // do stuff

    std::cout.rdbuf(coutbuf); //reset to standard output again
 */