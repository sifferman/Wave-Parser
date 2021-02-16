
<!-- README.md -->


# Wave-Parser

Repository: <https://github.com/E4tHam/Wave-Parser>

## About

This code will open a .wav file and store the data from its format and data chunks. It will also store and print other miscellaneous chunks, such as BEXT and _PMX.

It follows the file format layed out in <http://soundfile.sapp.org/doc/WaveFormat/>.

## Getting Started

To build, open the Makefile and ensure that the correct g++ compiler flags are used according to your OS. If you are on Windows, be sure to include the -lws2_32 flag.

Then run `make`.

To open a wave file, run `./main <FILENAME>`.
