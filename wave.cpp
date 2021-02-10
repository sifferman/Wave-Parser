
/* wave.cpp */


#include "wave.h"

#include <iomanip>
#include <fstream>
#include <iostream>
#include <math.h>


/* Endian Conversion */

#if defined _WIN32 || defined __CYGWIN__
// https://docs.microsoft.com/en-us/windows/win32/api/winsock2/
#include <winsock2.h>
#else
// https://pubs.opengroup.org/onlinepubs/7908799/xns/arpainet.h.html
#include <arpa/inet.h>
#endif
// host to little endian
#define __htole32(...) __builtin_bswap32(htonl(__VA_ARGS__))
#define __htole16(...) __builtin_bswap16(htons(__VA_ARGS__))
// little endian to host
#define __letoh32(...) __builtin_bswap32(ntohl(__VA_ARGS__))
#define __letoh16(...) __builtin_bswap16(ntohs(__VA_ARGS__))



class BadLoadSize { };

template <class T>
void load( T & destination, const char * const buffer, size_t & offset ) {
    size_t length = sizeof(destination);
    memcpy( &destination, buffer + offset, length );
    switch ( length ) {
    case 1:
        break;
    case 2:
        destination = __letoh16(destination);
        break;
    case 4:
        destination = __letoh32(destination);
        break;
    default:
        throw BadLoadSize();
        break;
    }
    offset += length;
}



Wave::Chunk::Chunk() : ChunkSize(0) {
    memset( ChunkID, 0, 4 );
}
Wave::Chunk::Chunk( const char * buffer, const size_t & buffer_size = -1 ) {
    if ( buffer_size < 8 )
        throw BadFormat();

    size_t offset = 0;

    memcpy( ChunkID, buffer + offset, 4 );
    offset += 4;

    load( ChunkSize, buffer, offset );
    
    print();
}
Wave::Chunk & Wave::Chunk::operator=( const Chunk & c ) {
    ChunkSize = c.ChunkSize;
    memcpy( ChunkID, c.ChunkID, 4 );
    return *this;
}
void Wave::Chunk::print() const {
    printf( "%c%c%c%c\n", ChunkID[0], ChunkID[1], ChunkID[2], ChunkID[3] );
    printf( "%d\n", ChunkSize );
}





Wave::fmt_Chunk::fmt_Chunk() 
: Chunk(),
  AudioFormat(0),
  NumChannels(0),
  SampleRate(0),
  ByteRate(0),
  BlockAlign(0),
  BitsPerSample(0)
{}
Wave::fmt_Chunk::fmt_Chunk( const char * buffer, const size_t & buffer_size ) 
: Chunk( buffer, buffer_size ) {
    if ( buffer_size < 16+8 || ChunkSize != 16 )
        throw BadFormat();

    size_t offset = 8;


    load( AudioFormat   , buffer, offset );
    if ( AudioFormat != 1 )
        throw BadFormat();

    load( NumChannels   , buffer, offset );

    load( SampleRate    , buffer, offset );

    load( ByteRate      , buffer, offset );

    load( BlockAlign    , buffer, offset );

    load( BitsPerSample , buffer, offset );
    if ( BitsPerSample & 0b111 || BitsPerSample > 32 )
        throw BadFormat();
}
Wave::fmt_Chunk & Wave::fmt_Chunk::operator=( const fmt_Chunk & c ) {
    (Chunk&)(*this) = c ;
    AudioFormat = c.AudioFormat;
    NumChannels = c.NumChannels;
    SampleRate = c.SampleRate;
    ByteRate = c.ByteRate;
    BlockAlign = c.BlockAlign;
    BitsPerSample = c.BitsPerSample;
    return *this;
}




Wave::dataChunk::dataChunk()
: Chunk(), data(0)
{ }
Wave::dataChunk::dataChunk( const char * buffer, const size_t & buffer_size )
: Chunk( buffer, buffer_size ) {
    data = new uint8_t[ ChunkSize ];
    memcpy( data, buffer + 8, ChunkSize );
}
Wave::dataChunk::~dataChunk() {
    if ( data )
        delete [] data;
    data = 0;
}
Wave::dataChunk & Wave::dataChunk::operator=( const dataChunk & c ) {
    (Chunk&)(*this) = c ;
    if ( data )
        delete [] data;
    data = new uint8_t[ ChunkSize ];
    memcpy( data, c.data, c.ChunkSize );
    return *this;
}
void Wave::dataChunk::print_data( size_t num ) const {
    for ( uint8_t i = 0; i < num; i++ )
        printf( "%#04x\n", data[i] );
}




constexpr const char Wave::MainChunk::WAVE_ChunkIDs[2][4];
constexpr const char Wave::MainChunk::WAVE_Format[4];

Wave::MainChunk::MainChunk() 
: Chunk(), fmt__Chunk(), data_Chunk() {
    memset( Format, 0, 4 );
}
Wave::MainChunk::MainChunk( const char * buffer, const size_t & buffer_size )
: Chunk( buffer, buffer_size ) {
    size_t offset = 8;

    // Format
    memcpy( Format, buffer + offset, 4 );
    offset += 4;

    // if the Format is Wave
    if ( memcmp( Format, WAVE_Format, 4 ) == 0 ) {

        // fmt chunk
        fmt__Chunk = fmt_Chunk( buffer + offset, buffer_size - offset );
        if ( memcmp( fmt__Chunk.ChunkID, WAVE_ChunkIDs[0], 4 ) != 0 ) throw BadFormat();
        offset += fmt__Chunk.ChunkSize + 8;

        // data chunk
        data_Chunk = dataChunk( buffer + offset, buffer_size - offset );
        if ( memcmp( data_Chunk.ChunkID, WAVE_ChunkIDs[1], 4 ) != 0 ) throw BadFormat();

    }
    else {
        throw BadFormat();
    }

}
Wave::MainChunk & Wave::MainChunk::operator=( const MainChunk & c ) {
    memcpy( Format, c.Format, 4 );
    fmt__Chunk = c.fmt__Chunk;
    data_Chunk = c.data_Chunk;
    return *this;
}




Wave::Wave( const std::string & filename ) {

    std::ifstream ifs( filename );

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

    file = MainChunk( buffer, buffer_size );

    BytesPerSample = file.fmt__Chunk.BitsPerSample >> 3;
}

void Wave::print() const {
    file.print();
}

uint32_t Wave::getSample( size_t i, uint16_t channel ) const {

    uint32_t out = 0;
    size_t offset = (BytesPerSample * ( i*file.fmt__Chunk.NumChannels + channel));

    if ( channel >= file.fmt__Chunk.NumChannels || offset > file.data_Chunk.ChunkSize - BytesPerSample )
        throw OutOfRange();

    memcpy(
        &out,
        file.data_Chunk.data + offset,
        BytesPerSample
    );

    out = __letoh32(out);
    
    // printf( "%d,%d : %#010x\n", channel, i, out );

    return out;
}
