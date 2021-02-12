
/* main.cpp */


#include "wave.h"

#include <iostream>
#include <fstream>

using namespace std;

class CouldNotOpenFile { };

int main() {

    Wave w( "TESTS/test_44.1.wav" );
    
    w.print();
    cout << "printing samples:\n" << endl;

    w.printSamples( 100 );
    
    return 0;
}
