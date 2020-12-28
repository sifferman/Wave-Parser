
/* wave.h */
/* http://soundfile.sapp.org/doc/WaveFormat/ */


#ifndef _WAVE_
#define _WAVE_

#include <cstddef>
#include <stdint.h>
#include <vector>
#include <string>


class Wave {
    public:
        Wave();
        Wave( const char * const buffer, const size_t & buffer_size );
        ~Wave();

        void smooth( unsigned char );

        void printFmt();
        void printData();
        void toFile( const std::string & filename );
    private:
        typedef std::vector<int16_t> data_vect;

        class BadFormat             { };

        class NoChannels            { };
        class CannotHandleNonPCM    { };
        class CannotHandleNon16Bit  { };

        uint16_t    NumChannels     ;
        uint32_t    SampleRate      ;
        uint32_t    ByteRate        ;
        uint16_t    BlockAlign      ;
        uint16_t    BitsPerSample   ; // must be 16 bit

        uint32_t    DataSize        ;
        data_vect * ChannelData     ;

        uint32_t get_MainChunkSize() {
            return 36 + DataSize;
        }
};

#endif
