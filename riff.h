
/* riff.h */

#ifndef _RIFF_H
#define _RIFF_H

#include <vector>
#include <stdint.h>
#include <cstdio>

namespace RIFF {

struct Chunk {
    // Exception for if the Chunk is the incorrect size.
    class BadChunkFormat { };

    // Creates empty Chunk.
    Chunk();

    // Loads Chunk from buffer.
    uint32_t load( const uint8_t * const buffer, const size_t buffer_size );

    // Writes Chunk data to buffer.
    uint32_t write( uint8_t * const buffer, const size_t buffer_size ) const;

    // Print the current Chunk ID and size.
    void print() const;

    
    uint8_t     ChunkID[4]  ; // Current Chunk ID.
    uint32_t    ChunkSize   ; // Current Chunk size.
};

struct MiscChunk : public Chunk {

    // Creates empty MiscChunk.
    MiscChunk();

    // Loads MiscChunk from buffer.
    uint32_t load( const uint8_t * const buffer, const size_t buffer_size );

    // Writes MiscChunk data to buffer.
    uint32_t write( uint8_t * const buffer, const size_t buffer_size ) const;

    // Destructor for MiscChunk.
    ~MiscChunk();

    // Prints all the data in ASCII form.
    void print_data( uint32_t num = -1 ) const;


    uint8_t * data; // Data for MiscChunk.
};

struct FormatChunk : public Chunk {

    // Creates empty FormatChunk.
    FormatChunk();

    // Loads FormatChunk from buffer.
    uint32_t load( const uint8_t * const buffer, const size_t buffer_size );

    // Writes FormatChunk data to buffer.
    uint32_t write( uint8_t * const buffer, const size_t buffer_size ) const;

    void printFormat() const;

    uint16_t    AudioFormat     ;   // PCM = 1 (i.e. Linear quantization)
    uint16_t    NumChannels     ;   // Mono = 1, Stereo = 2, etc.
    uint32_t    SampleRate      ;   // 8000, 44100, etc.
    uint32_t    ByteRate        ;   // == SampleRate * NumChannels * BitsPerSample/8
    uint16_t    BlockAlign      ;   // == NumChannels * BitsPerSample/8
    uint16_t    BitsPerSample   ;   // 8 bits = 8, 16 bits = 16, etc.

};

struct DataChunk : public MiscChunk {

    // Creates empty DataChunk.
    DataChunk();

    // Loads DataChunk from buffer.
    uint32_t load( const uint8_t * const buffer, const size_t buffer_size );

    // Writes DataChunk data to buffer.
    uint32_t write( uint8_t * const buffer, const size_t buffer_size ) const;

};

struct RiffChunk : public Chunk {
    
    // Exception for if ChunkID does not match the expected value.
    class BadRiffFormat { };

    // RIFF ChunkID Expected Value.
    static constexpr const uint8_t RIFF_ChunkID[4] = {'R','I','F','F'};

    // RIFF expected value for wave file.
    static constexpr const uint8_t WAVE_Format[4] = {'W','A','V','E'};

    // Wave expected values for subchunk IDs.
    static constexpr const uint8_t WAVE_ChunkIDs[2][4] = { {'f','m','t',' '}, {'d','a','t','a'} };

    // Creates empty RiffChunk.
    RiffChunk();

    // Destructor for RiffChunk.
    ~RiffChunk();

    // Loads RiffChunk from buffer.
    uint32_t load( const uint8_t * const buffer, const size_t buffer_size );

    // Writes RiffChunk data to buffer.
    uint32_t write( uint8_t * const buffer, const size_t buffer_size ) const;


    uint8_t         Format[4]   ;   // Contains the letters "WAVE".
    FormatChunk     formatChunk ;   // Describes the format the the sound information in dataChunk.
    DataChunk       dataChunk   ;   // Contains the raw sound data.

    std::vector<MiscChunk*> miscChunks;  // Contains all other chunks that are not the format or data chunks.
};

}

#endif
