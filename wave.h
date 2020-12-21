
/* wave.h */
/* http://soundfile.sapp.org/doc/WaveFormat/ */


#ifndef _WAVE_
#define _WAVE_

#include <cstddef>
#include <stdint.h>
#include <vector>


class Wave {
    public:
        Wave();
        Wave( const char * const buffer, const size_t & buffer_size );
        ~Wave();

        void printFmt();
        void printData();
    private:
        typedef std::vector<uint16_t> data_vect;

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

};

#endif
