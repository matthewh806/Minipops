<pre>
                __   __   __   __  
 |\/| | |\ | | |__) /  \ |__) /__` 
 |  | | | \| | |    \__/ |    .__/ 
                                   
</pre>

## ---------- ONLY TESTED AGAINST macOS High Sierra ----------
## ---------- USE AT YOUR OWN PERIL! ----------

## What is this
This is my personal tool for loading / deleting samples from my [Korg Volca Sample](https://www.korg.com/us/products/dj/volca_sample/)

There is some great 3rd party software out there for achieving the same (or similar), but of the ones I've tried each is lacking in some certain regard. Since Korg open sourced the [Syro SDK](http://korginc.github.io/volcasample/) and it is based on quite an interesting method of encoding the binary data and transfer I decided to write my own tool. 

Currently this is a command line app because the operations I want to perform can be easily and simply expressed with a few commands and therefore I have not built a GUI.

I have an active [Trello](https://trello.com/b/Xhqh0N8C/volcasampletool) board where I'm putting all my TODOs, bugs and ideas for improvements / features.

### Name
What is does the name Minipop mean or have to do with anything? Well, Korg's SDK is called Syro, which I think must be a reference to the aphex twin [album](https://en.wikipedia.org/wiki/Syro) of the same name. I love the track [minipops 67 [120.2]](https://open.spotify.com/track/6kzLbNxsJidjc80Nw3uZVg?si=kxSMP1xjToWOup7uhWUVqQ) and so I decided to steal the name :)

## Building
### Dependencies

I am currently making use of two libraries: 
- [SDL](http://www.libsdl.org/) for access to the system audio device. Install this with `brew install sdl2`

- [args](https://github.com/Taywee/args) which is a simple header-only C++ argument parser library (similar to [argparse](https://docs.python.org/3/library/argparse.html) in Python). Because this is a header only library, its included in its entirety in the Include directory.

- [spdlog](https://github.com/gabime/spdlog) a simple header-only C++ logging facility. I wanted to just include this in the include dir. as with args, but Xcode seemed to be having a problem finding the header files in subdirectories, even despite me telling it to search paths `$(PROJECT_DIRECTORY)/VolcaSampleTool/include` recursively. So, unfortunately this is a dependency for building: `brew install spdlog`

### Compiling
#### Xcode
The provided Xcode project should work out of the box. Just open and hit run!

#### Makefile
I've included a very simple Makefile, I've only tested against macOS and it uses both clang (for C compilation) and clang++ (for C++ compilation and C obj file linking).

Just run `make` at the command line and see what happens!


## Running examples
-- TODO -- 

## Transferring to Volca Sample
-- TODO -- 
