
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
        void printMiscChunkData( const uint32_t num_samples = -1 ) const;

        // Print format.
        void printFormat() const {
            file.formatChunk.printFormat();
        }

        // Returns the ith sample in the designated channel.
        uint32_t getSample( const uint32_t i, const uint16_t channel ) const;

        // Prints num_samples samples from the wave file.
        void printSamples( const uint32_t num_samples = -1 ) const;

        uint16_t GetAudioFormat() const {
            return file.formatChunk.AudioFormat;
        }
        uint16_t GetNumChannels() const {
            return file.formatChunk.NumChannels;
        }
        uint32_t GetSampleRate() const {
            return file.formatChunk.SampleRate;
        }
        uint32_t GetByteRate() const {
            return file.formatChunk.ByteRate;
        }
        uint16_t GetBlockAlign() const {
            return file.formatChunk.BlockAlign;
        }
        uint16_t GetBitsPerSample() const {
            return file.formatChunk.BitsPerSample;
        }
        const void * getSampleMemory() const {
            return file.dataChunk.data;
        }
        uint32_t getSampleMemorySize() const {
            return file.dataChunk.ChunkSize;
        }

    private:
        // Exception for if the input file could not be opened.
        class CouldNotOpenFile { };

        // Exception for if the input file is too large.
        class FileTooLarge { };

        // Exception for if the output file could not be created.
        class CouldNotCreateFile { };

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
