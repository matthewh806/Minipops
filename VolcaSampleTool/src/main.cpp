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
#include "args.hxx"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "syro_operations.hpp"

static uint8_t *audioPos;
static uint32_t audioLen;

auto console = spdlog::stdout_color_mt("main");

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
        console->error("Could not initialize SDL");
        return 1;
    }
    
    SDL_AudioSpec wavSpec;
    uint8_t* wavStart;
    uint32_t wavLength;
    
    if(SDL_LoadWAV(filename, &wavSpec, &wavStart, &wavLength) == NULL) {
        console->error("Error: File could not be loaded as an audio file");
        return 1;
    }
    
    wavSpec.callback = audioCallback;
    wavSpec.userdata = NULL;
    audioPos = wavStart;
    audioLen = wavLength;
    
    if(SDL_OpenAudio(&wavSpec, NULL) < 0) {
        console->error(SDL_GetError());
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
    std::unordered_map<std::string, commandType> map {
        {"add", Add},
        {"delete", Delete}
    };
    
    spdlog::set_level(spdlog::level::debug);
    
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
        console->info("Prepare for audio playback: {}", args::get(output).c_str());
        playbackAudio(args::get(output).c_str());
        console->info("Audio playback complete. GOODBYE <3");
        return 0;
    };

    
    console->info("Remember to transfer your sounds to your Volca <3");
    return 0;
}

void Delete(const std::string &prog_name, const char *output, std::vector<std::string>::const_iterator begin_args, std::vector<std::string>::const_iterator end_args)
{
    console->info("Leave me out to rot and die :(");
    
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
    console->info("Add me up and up and up!");
    
    args::ArgumentParser parser("");
    parser.Prog(prog_name + " add");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> samples(parser, "samples", "The sample(s) to load. Can be a file or directory (.wav format)");
    args::PositionalList<int> slots(parser, "slots", "The numbers of the sample slots to add the samples (0-based)");

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
