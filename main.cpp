
/* main.cpp */


#include "wave.h"

#include <iostream>
#include <fstream>

using namespace std;

class CouldNotOpenFile { };

int main( int argc, char * argv[] ) {

    if ( argc < 2 ) {
        cerr << "[ERROR]: Did not give file name." << endl;
        return 1;
    }

    Wave w( argv[1] );

    cout << "Printing Chunk Metadata:" << endl;
    w.printChunkMetadata();
    cout << endl;

    cout << "Printing samples:" << endl;
    w.printSamples(30);
    cout << endl;

    cout << "Printing misc. chunk data:" << endl;
    w.printMiscChunkData(100);
    cout << endl;

    w.printFormat();
    cout << endl;

    w.write( "test.wav" );

    cout << "Done." << endl;
    return 0;
}
