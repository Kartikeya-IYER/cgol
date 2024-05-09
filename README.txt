Conway's Game of Life in C99 using a bitfield
============================================================

This is a C99 implementation of Conway's Game of Life which uses a bitfield
of 64 bits to encode the game board in a 64-bit unsigned integer instead of
the usual implementation of a struct or integer array.

The game grid is an 8x8 toroidal grid which wraps around when its boundaries
are exceeded.

Using a bitfield not only saves memory, but - on a 64-bit system - also enables
using a memory register to store the 64-bit number representing the game board,
leading to potential speedups in evaluating the evolution of the game board,
since it enables using registers and also the CPU caches and pipeline in a
hopefully optimal manner.

This project was sparked after a conversation which got me thinking about
Conway's Game of Life, the lack of actual bit arrays in C, and the possibility 
of using a single number whose individual bits would represent the dead-or-alive
state of a Conway cell. So I thought: why not write a proof of concept to see
what such an implementation might look like...


Code, building, and running
----------------------------------
Indent style used is 1TBS, with tabs in the common FOSS project setup of
tabstop=4, shiftwidth=4, and with tabs expanded to spaces.

Build mechanisms for Linux as well as Windows are included.

Builds can be made in either "debug" mode or "release" mode. When built
in debug mode, extra debug information is printed when the binary is
executed.

Building on Linux:
    - debug mode: Run "make debug"
    - release mode: Run "make"

Building on Windows:
    - The included Visual Studio project has debug and release configurations.

When run without any command line parameters, the executable will generate a
random 8x8 game board and run it for 4 generations. To run with one of the
three currently-implemented standard patterns, run the executable with the
desired pattern as a command line parameter. It currently supports the "blinker",
"toad", and "beacon" standard patterns and executes them for 4 generations.

See the TODOs section below for some forthcoming improvements to this quick-and-dirty
proof of concept.


Files in this project:
--------------------------
.
├── cgol.c             -> main and command line parsing
├── cgol.h             -> defines and prototypes
├── cgol.sln           -> Visual Studio 2022 Project file
├── cgol.vcxproj       -> Visual Studio 2022 Project file
├── life.c             -> Game engine
├── life_tests.c       -> Runtime tests for game engine
├── Makefile           -> Linux Makefile
└── README.txt


TODOs
-----------------------
1. Support more standard patterns than just "blinker", "toad", and "beacon". 

2. Support running more than 4 generations, including an infinite run until
   the user chooses to terminate execution. Accept the number of generations
   via the command line.

3. Support displaying game board evolution not as discrete steps but as a single
   live board display which updates each generation on the same board.

4. Support a larger game board than 8x8 while still using the bitfield paradigm.

5. Allow users to specify a pattern of their choice.
