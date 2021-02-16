
/* wave.cpp */


#include "wave.h"

#include <fstream>
#include <cstring>

#include "wave_endian.h"





void Wave::load( const std::string & filename ) {

    std::ifstream ifs( filename, std::ofstream::binary );

    if ( !ifs )
        throw CouldNotOpenFile();

    // get length of file:
    ifs.seekg( 0, ifs.end );
    size_t buffer_size = ifs.tellg();
    ifs.seekg( 0, ifs.beg );

    // ensures the length is allowed
    if ( buffer_size > 0xFFFFFFFFULL ) {
        ifs.close();
        throw FileTooLarge();
    }

    // load into buffer
    uint8_t * buffer = new uint8_t[ buffer_size ];
    ifs.read( (char*)buffer, buffer_size );
    ifs.close();

    // Initialize file
    file.load( buffer, buffer_size );
    delete buffer;

    verify();

}

void Wave::write( const std::string & filename ) const {

    std::ofstream ofs( filename, std::ofstream::binary );

    if ( !ofs )
        throw CouldNotCreateFile();

    // get length of file:
    uint32_t buffer_size = file.ChunkSize + 8;

    // load into buffer
    uint8_t * buffer = new uint8_t[ buffer_size ];
    memset( buffer, 0, buffer_size );
    file.write( buffer, buffer_size );

    // write buffer into file
    ofs.write( (char*)buffer, buffer_size );

    ofs.close();
    delete buffer;

}










void Wave::verify() {
    // stop if the Format is not Wave
    if ( memcmp( file.Format, RIFF::RiffChunk::WAVE_Format, 4 ) != 0 )
        throw BadWaveFormat();

    // check that Fmt Chunk is correct size
    if ( file.formatChunk.ChunkSize != 16 )
        throw BadWaveFormat();

    // check for PCM
    if ( file.formatChunk.AudioFormat != 1 )
        throw BadWaveFormat();

    // check BitsPerSample
    if ( file.formatChunk.BitsPerSample & 0b111 || file.formatChunk.BitsPerSample > 32 )
        throw BadWaveFormat();
}














uint32_t Wave::getSample( const uint32_t i, const uint16_t channel ) const {

    if ( file.formatChunk.BitsPerSample > 32 )
        throw OutOfRange();

    uint8_t BytesPerSample = file.formatChunk.BitsPerSample >> 3;

    uint32_t out = 0;
    uint32_t offset = BytesPerSample * ( i*file.formatChunk.NumChannels + channel);

    if ( channel >= file.formatChunk.NumChannels || offset > file.dataChunk.ChunkSize - BytesPerSample )
        throw OutOfRange();

    memcpy(
        &out,
        file.dataChunk.data + offset,
        BytesPerSample
    );

    out = __letoh32(out);

    // printf( "%d,%d : %#010x\n", channel, i, out );

    return out;
}















void Wave::printChunkMetadata() const {
    file.print();
    printf("\nFormat Chunk:\n");
    file.formatChunk.print();
    printf("\nData Chunk:\n");
    file.dataChunk.print();
    printf( "\n%d Misc. Chunks:\n", (uint32_t) file.miscChunks.size() );
    for ( const auto c : file.miscChunks )
        c->print();
}

void Wave::printMiscChunkData( const uint32_t num ) const {
    for ( const auto c : file.miscChunks ) {
        c->print();
        c->print_data( num );
        printf("\n");
    }
}

void Wave::printSamples( const uint32_t num_samples ) const {
    try {
        for ( uint32_t sample = 0; sample < num_samples; sample++ ) {
            for ( uint32_t channel = 0; channel < file.formatChunk.NumChannels; channel++ ) {
                if ( channel != 0 )
                    printf(" ---- ");
                switch ( file.formatChunk.BitsPerSample ) {
                case 8:
                    printf( "%02d : %d   =  %#04x", sample, channel, getSample( sample, channel ) );
                    break;
                case 16:
                    printf( "%02d : %d   =  %#06x", sample, channel, getSample( sample, channel ) );
                    break;
                case 24:
                    printf( "%02d : %d   =  %#08x", sample, channel, getSample( sample, channel ) );
                    break;
                case 32:
                    printf( "%02d : %d   =  %#010x", sample, channel, getSample( sample, channel ) );
                    break;
                default:
                    throw OutOfRange();
                    break;
                }
            }
            printf("\n");
        }
    } catch ( OutOfRange e ) { }

}
