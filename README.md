
<!-- README.md -->


# Wave-Parser

Repository: <https://github.com/E4tHam/Wave-Parser>

## About

This code will open a .wav file and store the data from its format and data chunks. It will also store and print other miscellaneous chunks, such as BEXT and _PMX.

It follows the file format layed out in <http://soundfile.sapp.org/doc/WaveFormat/>.

## Getting Started

How to build:

1. Edit the Makefile so that the correct g++ compiler flags are used according to your OS. (If you are on Windows, be sure to include the -lws2_32 flag.)
2. Run `make`.

After building, run `./main <FILENAME>`.
