//
//  main.cpp
//  VolcaSampleTool
//
//  Created by Matthew on 18/09/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#include <iostream>
#include "korg_syro_volcasample.h"
#include "helper_functions.hpp"
#include <SDL2/SDL.h>
#include "../include/args.hxx" // TODO: Fix this path
#include "syro_operations.hpp"

static uint8_t *audioPos;
static uint32_t audioLen;

void audioCallback(void *userData, uint8_t *stream, int len)
{
    if(audioLen == 0)
        return;
    
    len = (len > audioLen) ? audioLen : len;
    SDL_memcpy(stream, audioPos, len);
    
    audioPos += len;
    audioLen -= len;
}

int playbackAudio(const char *filename)
{
    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Error: Could not initialize SDL" << std::endl;
        return 1;
    }
    
    SDL_AudioSpec wavSpec;
    uint8_t* wavStart;
    uint32_t wavLength;
    
    if(SDL_LoadWAV(filename, &wavSpec, &wavStart, &wavLength) == NULL) {
        std::cerr << "Error: File could not be loaded as an audio file" << std::endl;
        return 1;
    }
    
    wavSpec.callback = audioCallback;
    wavSpec.userdata = NULL;
    audioPos = wavStart;
    audioLen = wavLength;
    
    if(SDL_OpenAudio(&wavSpec, NULL) < 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_PauseAudio(0);
    
    while(audioLen > 0) {
        SDL_Delay(100);
    }
    
    SDL_CloseAudio();
    SDL_FreeWAV(wavStart);
    
    return 0;
}

void Add(const std::string &prog_name, const char *output, std::vector<std::string>::const_iterator begin_args, std::vector<std::string>::const_iterator end_args);
void Delete(const std::string &prog_name, const char *output, std::vector<std::string>::const_iterator begin_args, std::vector<std::string>::const_iterator end_args);

using commandType = std::function<void(const std::string &, const char *, std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator)>;

int main(int argc, const char * argv[]) {
    
    // TODO: Specify relative or full path at CLI
    // TODO: pass audio buffer straight to device without saving it? (pointlessly saving and then loading saved file...)
    // TODO: offset the value of the sample slots (currently only takes 0...x)
    // TODO: animated ascii while waiting for playback (ask kendal maybe?)
    // TODO: restore factory settings (will increase the download size massively unless dl'd from the internet)
    // TODO: logging facility
    // TODO: lots of things!!!! ^^^^^
    
    std::unordered_map<std::string, commandType> map {
        {"add", Add},
        {"delete", Delete}
    };
    
    const std::vector<std::string> args(argv + 1, argv + argc);
    args::ArgumentParser parser("Hello, welcome to VoLCA TOoOOoooOL!", "Valid commands are add and delete");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    parser.Prog(argv[0]);
    parser.ProglinePostfix("{Command options}");
    args::ValueFlag<std::string>output(parser, "output file", "the file to write the syrostream out to (wav)", {'o', "output"});
    args::Flag auto_play(parser, "auto play", "If this flag is set autoplay the generated syro stream", {'a', "auto"});
    args::MapPositional<std::string, commandType> command(parser, "command", "command to execute {add, delete}", map);
    command.KickOut(true);

    try
    {
        auto next = parser.ParseArgs(args);
        std::cout << std::boolalpha;
        
        if(command) {
            args::get(command)(argv[0], args::get(output).c_str(), next, std::end(args));
        } else {
            std::cout << parser;
            return 0;
        }
    } catch(args::Help) {
        std::cout << parser;
        return 0;
    } catch(args::Error& e) {
        std::cerr << e.what() << std::endl << parser;
        return 1;
    }

    if(auto_play) {
        std::cout << "Prepare for audio playback! " << args::get(output).c_str() << std::endl;
        return playbackAudio(args::get(output).c_str());
    };

    return 0;
}

void Delete(const std::string &prog_name, const char *output, std::vector<std::string>::const_iterator begin_args, std::vector<std::string>::const_iterator end_args)
{
    std::cout << " Leave me out to rot and die..." << std::endl;
    
    args::ArgumentParser parser("");
    parser.Prog(prog_name + " delete");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::PositionalList<int> slots(parser, "slots", "The numbers of the sample slots to delete (0-based)");
    
    try
    {
        parser.ParseArgs(begin_args, end_args);
        syro_operations::constructSyroStream(output, nullptr, DataType_Sample_Erase, args::get(slots));
    } catch(args::Help) {
        std::cout << parser;
        return;
    } catch(args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
}

void Add(const std::string &prog_name, const char *output, std::vector<std::string>::const_iterator begin_args, std::vector<std::string>::const_iterator end_args)
{
    std::cout << " Add me up and up and up!" << std::endl;
    args::ArgumentParser parser("");
    parser.Prog(prog_name + " add");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> samples(parser, "samples", "The sample(s) to load. Can be a file or directory (.wav format)");
    args::PositionalList<int> slots(parser, "slots", "The numbers of the sample slots to add the samples (0-based)");

    // TODO: Mutually exclusive arguments: individual files, directories.
    //
    // Pattern?
    // Directories should just take a start argument and end be determined by the number of files in dir
    // Individual files should be specified as filename.wav and index if possible?

    try
    {
        parser.ParseArgs(begin_args, end_args);
        syro_operations::constructSyroStream(output, args::get(samples).c_str(), DataType_Sample_Compress, args::get(slots));
    } catch(args::Help) {
        std::cout << parser;
        return;
    } catch(args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
}
