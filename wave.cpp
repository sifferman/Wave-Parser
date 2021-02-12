
/* wave.cpp */


#include "wave.h"

#include <fstream>


/* Endian Conversion */
#include "endian.h"


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
Wave::Chunk::Chunk( const char * const buffer, const size_t buffer_size ) {
    if ( buffer_size < 8 )
        throw BadChunkFormat();

    size_t offset = 0;

    // load ChunkID
    memcpy( ChunkID, buffer + offset, 4 );
    offset += 4;


    // load ChunkSize
    load( ChunkSize, buffer, offset );

    // check if the entire chunk is in buffer
    if ( buffer_size < 8 + ChunkSize )
        throw BadChunkFormat();

}
Wave::Chunk & Wave::Chunk::operator=( const Chunk & c ) {

    // copy ChunkID
    memcpy( ChunkID, c.ChunkID, 4 );
    // copy ChunkSize
    ChunkSize = c.ChunkSize;

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
Wave::fmt_Chunk::fmt_Chunk( const char * const buffer, const size_t buffer_size )
: Chunk( buffer, buffer_size ) {

    size_t offset = 8;

    // load AudioFormat
    load( AudioFormat   , buffer, offset );

    // load NumChannels
    load( NumChannels   , buffer, offset );

    // load SampleRate
    load( SampleRate    , buffer, offset );

    // load ByteRate
    load( ByteRate      , buffer, offset );

    // load BlockAlign
    load( BlockAlign    , buffer, offset );

    // load BitsPerSample
    load( BitsPerSample , buffer, offset );
    
}
Wave::fmt_Chunk & Wave::fmt_Chunk::operator=( const fmt_Chunk & c ) {
    // copy ChunkID and ChunkSize
    (Chunk&)(*this) = c ;

    AudioFormat     = c.AudioFormat     ;
    NumChannels     = c.NumChannels     ;
    SampleRate      = c.SampleRate      ;
    ByteRate        = c.ByteRate        ;
    BlockAlign      = c.BlockAlign      ;
    BitsPerSample   = c.BitsPerSample   ;

    return *this;
}




Wave::dataChunk::dataChunk()
: Chunk(), data(0)
{ }
Wave::dataChunk::dataChunk( const char * const buffer, const size_t buffer_size )
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
    // copy ChunkID and ChunkSize
    (Chunk&)(*this) = c ;

    // copy data
    if ( data )
        delete [] data;
    data = new uint8_t[ ChunkSize ];
    memcpy( data, c.data, c.ChunkSize );

    return *this;
}
void Wave::dataChunk::print_data( uint32_t num = -1 ) const {
    num = std::max( ChunkSize, num );
    for ( uint8_t i = 0; i < num; i++ )
        printf( "%c", data[i] );
}




constexpr const char Wave::RiffChunk::RIFF_Format[4];
constexpr const char Wave::WAVE_ChunkIDs[2][4];
constexpr const char Wave::WAVE_Format[4];

Wave::RiffChunk::RiffChunk()
: Chunk(), fmt__Chunk(), data_Chunk() {
    memset( Format, 0, 4 );
}
Wave::RiffChunk::RiffChunk( const char * const buffer, const size_t buffer_size )
: Chunk( buffer, buffer_size ) {

    if ( memcmp( ChunkID, RIFF_Format, 4 ) != 0 )
        throw BadRiffFormat();

    size_t offset = 8;

    // Format
    memcpy( Format, buffer + offset, 4 );
    offset += 4;


    while ( offset < ChunkSize + 8 ) {
        Chunk c( buffer + offset, buffer_size - offset );

        // if the ID is "fmt "
        if ( memcmp( c.ChunkID, WAVE_ChunkIDs[0], 4 ) == 0 ) {
            fmt__Chunk = fmt_Chunk( buffer + offset, buffer_size - offset );
        }
        // if the ID is "data"
        else if ( memcmp( c.ChunkID, WAVE_ChunkIDs[1], 4 ) == 0 ) {
            data_Chunk = dataChunk( buffer + offset, buffer_size - offset );
        }
        else {
            miscChunks.emplace_back( buffer + offset, buffer_size - offset );
        }

        offset += c.ChunkSize + 8;

    }

}
Wave::RiffChunk & Wave::RiffChunk::operator=( const RiffChunk & c ) {
    (Chunk&)(*this) = c ;
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

    file = RiffChunk( buffer, buffer_size );
    delete buffer;


    // stop if the Format is not Wave
    if ( memcmp( file.Format, WAVE_Format, 4 ) != 0 )
        throw BadWaveFormat();

    // check that Fmt Chunk is correct size
    if ( file.fmt__Chunk.ChunkSize != 16 )
        throw BadWaveFormat();

    // check for PCM
    if ( file.fmt__Chunk.AudioFormat != 1 )
        throw BadWaveFormat();

    // check BitsPerSample
    if ( file.fmt__Chunk.BitsPerSample & 0b111 || file.fmt__Chunk.BitsPerSample > 32 )
        throw BadWaveFormat();

}

void Wave::print() const {
    file.print();
    file.fmt__Chunk.print();
    file.data_Chunk.print();
    for ( const Chunk & c : file.miscChunks )
        c.print();
}

uint32_t Wave::getSample( const size_t i, const uint16_t channel ) const {

    uint8_t BytesPerSample = file.fmt__Chunk.BitsPerSample >> 3;

    uint32_t out = 0;
    size_t offset = BytesPerSample * ( i*file.fmt__Chunk.NumChannels + channel);

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

void Wave::printSamples( const size_t num_samples ) const {
    try {
        for ( size_t sample = 0; sample < num_samples; sample++ ) {
            for ( size_t channel = 0; channel < file.fmt__Chunk.NumChannels; channel++ ) {
                if ( channel != 0 )
                    printf(" ---- ");
                printf( "%02d : %d   =  %#06x", sample, channel, getSample( sample, channel ) );
            }
            printf("\n");
        }
    } catch ( OutOfRange e ) {
        printf("Requested too many samples.\n");
    }

}
