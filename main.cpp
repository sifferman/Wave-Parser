
/* main.cpp */


#include "wave.h"

#include <iostream>
#include <fstream>

using namespace std;

class CouldNotOpenFile { };

int main() {

    Wave w( "TESTS/test_44.1.wav" );
    w.getSample( 4, 0 );
    
    return 0;
}