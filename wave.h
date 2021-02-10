
/* wave.h */
/* http://soundfile.sapp.org/doc/WaveFormat/ */


#ifndef _WAVE_H_
#define _WAVE_H_

#include <cstddef>
#include <stdint.h>
#include <cstring>
#include <string>



class Wave {
    public:
        Wave( const std::string & filename );
        void print() const;
        uint32_t getSample( size_t i, uint16_t channel ) const;
        
    private:
        class CouldNotOpenFile { };
        class BadFormat { };
        class OutOfRange { };

        struct Chunk {
            Chunk();
            Chunk( const char * buffer, const size_t & buffer_size );
            Chunk & operator=( const Chunk & c );

            void print() const;

            char            ChunkID[4]  ;
            uint32_t        ChunkSize   ;
        };

        struct fmt_Chunk : public Chunk {
            fmt_Chunk();
            fmt_Chunk( const char * buffer, const size_t & buffer_size );
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
            dataChunk( const char * buffer, const size_t & buffer_size );
            dataChunk & operator=( const dataChunk & c );
            ~dataChunk();

            void print_data( size_t num ) const;
            
            uint8_t         * data      ;
        };

        struct MainChunk : public Chunk {
            static constexpr const char WAVE_ChunkIDs[2][4] = { {'f','m','t',' '}, {'d','a','t','a'} };
            static constexpr const char WAVE_Format[4] = {'W','A','V','E'};
            
            MainChunk();
            MainChunk( const char * buffer, const size_t & buffer_size );
            MainChunk & operator=( const MainChunk & c );

            char            Format[4]   ;
            fmt_Chunk       fmt__Chunk  ;
            dataChunk       data_Chunk  ;
        };


        MainChunk file;

        uint8_t BytesPerSample;
};

#endif
