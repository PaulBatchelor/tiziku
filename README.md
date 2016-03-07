# Tiziku

![tiziku](shot.png)

Tiziku is an audiovisual generative composition written for my 220b final 
project at Stanford University. It is also an experiment to get many
domain specific languages working together.


Tiziku uses the following libraries and languages:

- Chuck for timing (included in source code)
- Sporth for the main synthesis engine (run inside of ChucK)
- JACK for the audio engine
- NanoVG for vector graphics (included in source code)
- Lua 5.3 for scripting (primarily graphics generation)

## Setup

Tiziku is built for Linux. 

The following packages will need to be compiled and/or installed:

- Soundpipe (dev branch)
- Sporth
- Chorth (available in Sporth source code)
- JACK
- glfw

A custom version of ChucK is included in the Tiziku source code, and must
be compiled before compiling tiziku. Go into /usr/lib/chuck and run 
"make linux-jack".

In the top level directory, run "make" and things will (hopefully) compile.

Make sure the JACK server is running, and then run "./tiziku".

