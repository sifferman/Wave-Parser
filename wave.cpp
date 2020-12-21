
/* wave.cpp */


#include "wave.h"

#include "helpers.h"
#include <iomanip>

using namespace std;

Wave::Wave() {
    NumChannels     = 0 ;
    SampleRate      = 0 ;
    ByteRate        = 0 ;
    BlockAlign      = 0 ;
    BitsPerSample   = 0 ;
    DataSize        = 0 ;
    ChannelData     = 0 ;
}

Wave::Wave( const char * const buffer, const size_t & buffer_size ) {

    if ( buffer_size < 44 )
        throw BadFormat();

    size_t offset   = 0 ;

    /* Main Chunk */
    if (    buffer[offset]   != 'R' ||
            buffer[offset+1] != 'I' ||
            buffer[offset+2] != 'F' ||
            buffer[offset+3] != 'F' ) {
        throw BadFormat();
    }
    offset += 4;


    /* Main Chunk Size */
    uint32_t MainChunkSize;
    MainChunkSize = endian_swap<uint32_t>( buffer + offset );
    if ( MainChunkSize + 8 > buffer_size )
        throw BadFormat();
    offset += 4;

    /* Assert Format */
    if (    buffer[offset]   != 'W' ||
            buffer[offset+1] != 'A' ||
            buffer[offset+2] != 'V' ||
            buffer[offset+3] != 'E' ) {
        throw BadFormat();
    }
    offset += 4;

    /* Assert Chunk 1 is "fmt " */
    if (    buffer[offset]   != 'f' ||
            buffer[offset+1] != 'm' ||
            buffer[offset+2] != 't' ||
            buffer[offset+3] != ' ' ) {
        throw BadFormat();
    }
    offset += 4;

    /* Subchunk1Size */
    uint32_t Subchunk1Size;
    Subchunk1Size = endian_swap<uint32_t>( buffer + offset );
    if ( Subchunk1Size != 16 )
        throw CannotHandleNonPCM();
    offset += 4;


    /* AudioFormat */
    uint16_t AudioFormat;
    AudioFormat  = endian_swap<uint16_t>( buffer + offset );
    if ( AudioFormat != 1 )
        throw CannotHandleNonPCM();
    offset += 2;

    
    /* NumChannels */
    NumChannels = endian_swap<uint16_t>( buffer + offset );
    if ( NumChannels == 0 )
        throw NoChannels();
    ChannelData = new data_vect[NumChannels];
    offset += 2;

    /* SampleRate */
    SampleRate = endian_swap<uint32_t>( buffer + offset );
    offset += 4;
    
    /* ByteRate */
    ByteRate = endian_swap<uint32_t>( buffer + offset );
    offset += 4;

    /* BlockAlign */
    BlockAlign = endian_swap<uint16_t>( buffer + offset );
    offset += 2;

    /* BitsPerSample */
    BitsPerSample = endian_swap<uint16_t>( buffer + offset );
    if ( BitsPerSample != 16 )
        throw CannotHandleNon16Bit();
    offset += 2;

    /* Find Data SubChunk */
    while ( buffer[offset]   != 'd' ||
            buffer[offset+1] != 'a' ||
            buffer[offset+2] != 't' ||
            buffer[offset+3] != 'a' ) {
        offset++;
        if ( offset > MainChunkSize + 8 )
            throw BadFormat();
    }
    offset += 4;

    /* Data Size */
    DataSize = endian_swap<uint32_t>( buffer + offset );
    offset += 4;
    if ( DataSize > MainChunkSize + 8 - offset )
        throw BadFormat();

    /* ChannelData */
    for ( size_t i = 0; i < DataSize * 8/BitsPerSample; i++ ) {
        // cerr <<  "0x" << std::hex << (offset + (i*2)) << '\n';
        ChannelData[ i % NumChannels ]
            .push_back (
                endian_swap<uint16_t>( buffer + offset + (i*2) )
            );
    }

}

Wave::~Wave() {
    if ( ChannelData )
        delete [] ChannelData;
    ChannelData = 0;
}





void Wave::printFmt() {
    cout << "NumChannels  : " << NumChannels << endl;
    cout << "SampleRate   : " << SampleRate << endl;
    cout << "ByteRate     : " << ByteRate << endl;
    cout << "BlockAlign   : " << BlockAlign << endl;
    cout << "BitsPerSample: " << BitsPerSample << endl;
}


void Wave::printData() {
    for ( uint32_t i = 0; i < ((DataSize/NumChannels) / (BitsPerSample/8)); i++ ) {
        for ( uint16_t channel = 0; channel < NumChannels; channel++ ) {
            cout
                << setfill('0') << setw( BitsPerSample/4 ) << std::hex
                << ChannelData[ channel ].at(i)
                << " ";
        }
        cout << endl;
    }
}
