
/* helpers.h */


#ifndef _HELPERS_
#define _HELPERS_

#include <stdint.h>
#include <iostream>

using namespace std;

template <class T>
T endian_swap( const void * const p ) {

    T               out     ;
    uint8_t         size    = sizeof( out )     ;

    const uint8_t * in_p    = (uint8_t*) p      ;
    uint8_t       * out_p   = (uint8_t*) (&out) ;
    
    for ( uint8_t i = 0; i < size; i++ ) {
        out_p[ i ] = in_p[ i ];
    }
    
    return out;
}

#endif
