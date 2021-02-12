
/* wave.h */
/* http://soundfile.sapp.org/doc/WaveFormat/ */


#ifndef _WAVE_H_
#define _WAVE_H_

#include <string>
#include <vector>



class Wave {
    public:
        Wave( const std::string & filename );
        void print() const;
        uint32_t getSample( const size_t i, const uint16_t channel ) const;
        void printSamples( const size_t num_samples ) const;

    private:
        class CouldNotOpenFile { };
        class BadWaveFormat { };
        class OutOfRange { };

        static constexpr const char WAVE_ChunkIDs[2][4] = { {'f','m','t',' '}, {'d','a','t','a'} };
        static constexpr const char WAVE_Format[4] = {'W','A','V','E'};

        struct Chunk {
            class BadChunkFormat { };

            Chunk();
            Chunk( const char * const buffer, const size_t buffer_size );
            Chunk & operator=( const Chunk & c );

            void print() const;

            char            ChunkID[4]  ;
            uint32_t        ChunkSize   ;
        };

        struct fmt_Chunk : public Chunk {

            fmt_Chunk();
            fmt_Chunk( const char * const buffer, const size_t buffer_size );
            fmt_Chunk & operator=( const fmt_Chunk & c );

            uint16_t    AudioFormat     ;
            uint16_t    NumChannels     ;
            uint32_t    SampleRate      ;
            uint32_t    ByteRate        ;
            uint16_t    BlockAlign      ;
            uint16_t    BitsPerSample   ;
        };

        struct dataChunk : public Chunk {

            dataChunk();
            dataChunk( const char * const buffer, const size_t buffer_size );
            dataChunk & operator=( const dataChunk & c );
            ~dataChunk();

            void print_data( uint32_t num ) const;

            uint8_t * data;
        };

        struct RiffChunk : public Chunk {
            class BadRiffFormat { };

            static constexpr const char RIFF_Format[4] = {'R','I','F','F'};

            RiffChunk();
            RiffChunk( const char * const buffer, const size_t buffer_size );
            RiffChunk & operator=( const RiffChunk & c );

            char            Format[4]   ;
            fmt_Chunk       fmt__Chunk  ;
            dataChunk       data_Chunk  ;

            std::vector<dataChunk> miscChunks;
        };

        RiffChunk file;
};

#endif
