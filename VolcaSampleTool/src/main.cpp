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

#define WAV_POS_RIFF_SIZE 0x04
#define WAV_POS_DATA_SIZE 0x28

static void freeSyroData(SyroData *syro_data, int num_of_data)
{
    int i;
    
    for(i=0; i < num_of_data; i++)
    {
        if(syro_data->pData) {
            free(syro_data->pData);
            syro_data->pData = NULL;
        }
        
        syro_data++;
    }
}

static void constructDeleteData(SyroData *syro_data)
{
    syro_data->DataType = DataType_Sample_Erase;
    syro_data->pData = NULL;
    syro_data->Number = 0;
    syro_data->Size = 0;
    
    syro_data++;
}

static int constructSyroStream(const char *filename)
{
    // 1.prepare the data to be converted
    // 2.call the conversion start function
    // 3.process syrodata for each frame
    // 4.call the conversion end function
    
    SyroData syroData[10];
    SyroStatus status;
    SyroHandle handle;
    int numOfData = 1;
    uint32_t frame;
    uint8_t *buf_dest;
    uint32_t size_dest, write_pos;
    int16_t left, right;
    
    constructDeleteData(syroData);
    
    status = SyroVolcaSample_Start(&handle, syroData, numOfData, 0, &frame);
    if(status != Status_Success) {
        printf(" Start error, %d \n", status);
        freeSyroData(syroData, numOfData);
        
        return 1;
    }
    
    size_dest = (frame * 4) + sizeof(volca_constants::wav_header); // TOOD: why x 4?
    
    buf_dest = (uint8_t*) malloc(size_dest);
    if(!buf_dest) {
        printf(" Not enough memory for wtite file.\n");
        SyroVolcaSample_End(handle);
        freeSyroData(syroData, numOfData);
        
        return 1;
    }
    
    memcpy(buf_dest, volca_constants::wav_header, sizeof(volca_constants::wav_header));
    volca_helper_functions::set32BitValue((buf_dest + WAV_POS_RIFF_SIZE), (frame * 4 + 0x24));
    volca_helper_functions::set32BitValue((buf_dest + WAV_POS_DATA_SIZE), (frame * 4));
    
    write_pos = sizeof(volca_constants::wav_header);
    while(frame) {
        SyroVolcaSample_GetSample(handle, &left, &right);
        buf_dest[write_pos++] = (uint8_t)left;
        buf_dest[write_pos++] = (uint8_t)(left >> 8);
        buf_dest[write_pos++] = (uint8_t)right;
        buf_dest[write_pos++] = (uint8_t)(right >> 8);
        frame--;
    }
    
    SyroVolcaSample_End(handle);
    freeSyroData(syroData, numOfData);
    
    if(volca_helper_functions::writeFile(filename, buf_dest, size_dest))
        printf("Conversion complete\n");
    
    free(buf_dest);
    
    return 0;
}

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

int main(int argc, const char * argv[]) {
    
    args::ArgumentParser parser("Simple tool for transferring samples to Korg Volca Sample");
    args::Group commands(parser, "commands");
    args::Command add(commands, "load", "Loads a single or multiple samples onto device");
    args::Command remove(commands, "remove", "Remove a single or multiple samples from device");
    args::HelpFlag help(parser, "help", "", {'h', "help"});
    args::CompletionFlag completion(parser, {"complete"});
    
    try
    {
        parser.ParseCLI(argc, argv);
        
        if(add) {
            std::cout << "I haven't added (haha!) this yet..." << std::endl;
            return 1;
        } else {
            constructSyroStream("output.wav"); // TODO: Retrieve this from CL
        }
    } catch(args::Help) {
        std::cout << parser;
        return 1;
    } catch(args::Error& e) {
        std::cerr << e.what() << std::endl << parser;
        return 1;
    }

//    if(constructSyroStream("output.wav")) {
//        printf("Error constructing SyroStream! \n");
//        return 1;
//    }
    
    return playbackAudio("output.wav");
}
