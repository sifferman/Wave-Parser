
/* riff.cpp */

#include "riff.h"

#include <cstring>
#include <cstdio>

#include "wave_endian.h"












/*=================== struct Chunk =====================*/


RIFF::Chunk::Chunk() : ChunkSize(0) {
    memset( ChunkID, 0, 4 );
}
uint32_t RIFF::Chunk::load( const uint8_t * const buffer, const size_t buffer_size ) {
    if ( buffer_size < 8 )
        throw BadChunkFormat();

    uint32_t offset = 0;

    // load ChunkID
    memcpy( ChunkID, buffer + offset, 4 );
    offset += 4;


    // load ChunkSize
    load_le( ChunkSize, buffer, offset );

    // check if the entire chunk is in buffer
    if ( buffer_size < 8 + ChunkSize )
        throw BadChunkFormat();

    return 8 + ChunkSize;

}
uint32_t RIFF::Chunk::write( uint8_t * const buffer, const size_t buffer_size ) const {
    if ( buffer_size < 8 )
        throw BadChunkFormat();

    uint32_t offset = 0;

    // load ChunkID
    memcpy( buffer + offset, ChunkID, 4 );
    offset += 4;


    // load ChunkSize
    write_le( ChunkSize, buffer, offset );

    // check if the entire chunk is in buffer
    if ( buffer_size < offset + ChunkSize )
        throw BadChunkFormat();

    return 8 + ChunkSize;

}
void RIFF::Chunk::print() const {
    printf( "\"%c%c%c%c\" : ", ChunkID[0], ChunkID[1], ChunkID[2], ChunkID[3] );
    printf( "%d\n", ChunkSize );
}











/*=================== struct MiscChunk : public Chunk =====================*/

RIFF::MiscChunk::MiscChunk()
: Chunk(), data(0)
{ }
uint32_t RIFF::MiscChunk::load( const uint8_t * const buffer, const size_t buffer_size ) {

    uint32_t size = ((Chunk*)(this))->load( buffer, buffer_size );

    if ( data )
        delete [] data;

    data = new uint8_t[ ChunkSize ];
    memcpy( data, buffer + 8, ChunkSize );

    return size;
}
uint32_t RIFF::MiscChunk::write( uint8_t * const buffer, const size_t buffer_size ) const {

    uint32_t size = ((Chunk*)(this))->write( buffer, buffer_size );

    memcpy( buffer + 8, data, ChunkSize );

    return size;
}
RIFF::MiscChunk::~MiscChunk() {
    if ( data )
        delete [] data;
    data = 0;
}
void RIFF::MiscChunk::print_data( uint32_t num ) const {
    num = std::min( ChunkSize, num );
    for ( uint64_t i = 0; i < num; i++ )
        printf( "%c", data[i] );
    printf("\n");
}











/*=================== struct FormatChunk : public Chunk =====================*/

RIFF::FormatChunk::FormatChunk()
: Chunk(),
  AudioFormat(0),
  NumChannels(0),
  SampleRate(0),
  ByteRate(0),
  BlockAlign(0),
  BitsPerSample(0)
{}
uint32_t RIFF::FormatChunk::load( const uint8_t * const buffer, const size_t buffer_size ) {

    uint32_t size = ((Chunk*)(this))->load( buffer, buffer_size );
    uint32_t offset = 8;

    // load AudioFormat
    load_le( AudioFormat   , buffer, offset );

    // load NumChannels
    load_le( NumChannels   , buffer, offset );

    // load SampleRate
    load_le( SampleRate    , buffer, offset );

    // load ByteRate
    load_le( ByteRate      , buffer, offset );

    // load BlockAlign
    load_le( BlockAlign    , buffer, offset );

    // load BitsPerSample
    load_le( BitsPerSample , buffer, offset );

    return size;

}
uint32_t RIFF::FormatChunk::write( uint8_t * const buffer, const size_t buffer_size ) const {

    uint32_t size = ((Chunk*)(this))->write( buffer, buffer_size );
    uint32_t offset = 8;

    // write AudioFormat
    write_le( AudioFormat   , buffer, offset );

    // write NumChannels
    write_le( NumChannels   , buffer, offset );

    // write SampleRate
    write_le( SampleRate    , buffer, offset );

    // write ByteRate
    write_le( ByteRate      , buffer, offset );

    // write BlockAlign
    write_le( BlockAlign    , buffer, offset );

    // write BitsPerSample
    write_le( BitsPerSample , buffer, offset );

    return size;

}
void RIFF::FormatChunk::printFormat() const {
    printf("AudioFormat   : %d\n", AudioFormat    );
    printf("NumChannels   : %d\n", NumChannels    );
    printf("SampleRate    : %d\n", SampleRate     );
    printf("ByteRate      : %d\n", ByteRate       );
    printf("BlockAlign    : %d\n", BlockAlign     );
    printf("BitsPerSample : %d\n", BitsPerSample  );
}










/*=================== struct DataChunk : public MiscChunk =====================*/

RIFF::DataChunk::DataChunk()
: MiscChunk()
{ }

uint32_t RIFF::DataChunk::load( const uint8_t * const buffer, const size_t buffer_size ) {
    return ((MiscChunk*)(this))->load( buffer, buffer_size );
}
uint32_t RIFF::DataChunk::write( uint8_t * const buffer, const size_t buffer_size ) const {
    return ((MiscChunk*)(this))->write( buffer, buffer_size );
}










/*=================== struct RiffChunk : public Chunk =====================*/

constexpr const uint8_t RIFF::RiffChunk::RIFF_ChunkID[4];
constexpr const uint8_t RIFF::RiffChunk::WAVE_ChunkIDs[2][4];
constexpr const uint8_t RIFF::RiffChunk::WAVE_Format[4];

RIFF::RiffChunk::RiffChunk()
: Chunk(), formatChunk(), dataChunk() {
    memset( Format, 0, 4 );
}
uint32_t RIFF::RiffChunk::load( const uint8_t * const buffer, const size_t buffer_size ) {

    uint32_t size = ((Chunk*)(this))->load( buffer, buffer_size );

    if ( memcmp( ChunkID, RIFF_ChunkID, 4 ) != 0 )
        throw BadRiffFormat();

    uint32_t offset = 8;

    // Format
    memcpy( Format, buffer + offset, 4 );
    offset += 4;


    while ( offset < ChunkSize + 8 ) {
        Chunk c;
        uint32_t subsize = c.load( buffer + offset, buffer_size - offset );

        // if the ID is "fmt "
        if ( memcmp( c.ChunkID, WAVE_ChunkIDs[0], 4 ) == 0 ) {
            formatChunk.load( buffer + offset, buffer_size - offset );
        }
        // if the ID is "data"
        else if ( memcmp( c.ChunkID, WAVE_ChunkIDs[1], 4 ) == 0 ) {
            dataChunk.load( buffer + offset, buffer_size - offset );
        }
        else {
            miscChunks.push_back( new MiscChunk() );
            miscChunks.back()->load( buffer + offset, buffer_size - offset );
        }

        offset += subsize;

    }


    return size;
}
uint32_t RIFF::RiffChunk::write( uint8_t * const buffer, const size_t buffer_size ) const {

    uint32_t size = ((Chunk*)(this))->write( buffer, buffer_size );

    if ( memcmp( ChunkID, RIFF_ChunkID, 4 ) != 0 )
        throw BadRiffFormat();

    uint32_t offset = 8;

    // Format
    memcpy( buffer + offset, Format, 4 );
    offset += 4;

    offset += formatChunk.write( buffer + offset, buffer_size - offset );
    offset += dataChunk.write( buffer + offset, buffer_size - offset );

    for ( const auto c : miscChunks ) {
        offset += c->write( buffer + offset, buffer_size - offset );
    }

    return size;
}