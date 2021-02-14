
/* wave.h */
/* http://soundfile.sapp.org/doc/WaveFormat/ */


#ifndef _WAVE_H_
#define _WAVE_H_

#include "riff.h"

#include <string>
#include <vector>



class Wave {
    public:
        // Default Constructor
        Wave() { }
        // Loads wave data from a file.
        Wave( const std::string & filename ) {
            load( filename );
        }
        // Loads wave data from a file.
        void load( const std::string & filename );

        // Writes wave data to a file.
        void write( const std::string & filename ) const;

        // Print all subchunk IDs and sizes.
        void printChunkMetadata() const;

        // Print all MiscChunk data.
        void printMiscChunkData( const size_t num_samples = -1 ) const;

        // Print format.
        void printFormat() const {
            file.formatChunk.printFormat();
        }

        // Returns the ith sample in the designated channel.
        uint32_t getSample( const size_t i, const uint16_t channel ) const;

        // Prints num_samples samples from the wave file.
        void printSamples( const size_t num_samples = -1 ) const;

    private:
        // Exception for if the input file could not be opened.
        class CouldNotOpenFile { };

        // Exception for if the input file was not a wave file.
        class BadWaveFormat { };

        // Exception for if a requested sample was out of range.
        class OutOfRange { };

        // Ensure that file is a wave.
        void verify();
        
        // Holds all the data for the wave file.
        RIFF::RiffChunk file;
};

#endif
