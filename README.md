# VolcaSampleTool

## What is this
This is my personal tool for loading / deleting samples from my [Korg Volca Sample](https://www.korg.com/us/products/dj/volca_sample/)

There is some great 3rd party software out there for achieving the same (or similar), but of the ones I've tried each is lacking in some certain regard. Since Korg open sourced the [Syro SDK](http://korginc.github.io/volcasample/) and it is based on quite an interesting method of encoding the binary data and transfer I decided to write my own tool. 

Currently this is a command line app because the operations I want to perform can be easily and simply expressed with a few commands and therefore I have built a GUI.

## Building
### Dependencies

I am currently making use of two libraries: 
- [SDL](http://www.libsdl.org/) for access to the system audio device. Install this with `brew install sdl2`

- [args](https://github.com/Taywee/args) which is a simple header-only C++ argument parser library (similar to [argparse](https://docs.python.org/3/library/argparse.html) in Python). Because this is a header only library, its included in its entirety in the Include directory.

## Running examples
-- TODO -- 

## Transferring to Volca Sample
-- TODO -- 
