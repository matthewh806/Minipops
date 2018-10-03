//
//  helper_functions.hpp
//  VolcaSampleTool
//
//  Created by Matthew on 23/09/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#ifndef helper_functions_hpp
#define helper_functions_hpp

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <regex>
#include "spdlog/sinks/stdout_color_sinks.h"

#define WAVFMT_POS_ENCODE    0x00
#define WAVFMT_POS_CHANNEL    0x02
#define WAVFMT_POS_FS        0x04
#define WAVFMT_POS_BIT        0x0E

#define WAV_POS_RIFF_SIZE 0x04
#define WAV_POS_WAVEFMT   0x08
#define WAV_POS_DATA_SIZE 0x28

namespace volca_constants {
    static const uint8_t wav_header[] = {
        'R' , 'I' , 'F',  'F',        // 'RIFF'
        0x00, 0x00, 0x00, 0x00,        // Size (data size + 0x24)
        'W',  'A',  'V',  'E',        // 'WAVE'
        'f',  'm',  't',  ' ',        // 'fmt '
        0x10, 0x00, 0x00, 0x00,        // Fmt chunk size
        0x01, 0x00,                    // encode(wav)
        0x02, 0x00,                    // channel = 2
        0x44, 0xAC, 0x00, 0x00,        // Fs (44.1kHz)
        0x10, 0xB1, 0x02, 0x00,        // Bytes per sec (Fs * 4)
        0x04, 0x00,                    // Block Align (2ch,16Bit -> 4)
        0x10, 0x00,                    // 16Bit
        'd',  'a',  't',  'a',        // 'data'
        0x00, 0x00, 0x00, 0x00        // data size(bytes)
    };
}

namespace volca_helper_functions {
    extern uint16_t get16BitValue(uint8_t *ptr);
    extern uint32_t get32BitValue(uint8_t *ptr);
    extern void set32BitValue(uint8_t *ptr, uint32_t dat);
    extern bool isRegularFile(const char *path);
    extern bool isDirectory(const char *path);
    extern void readDirectory(const char *dirname, std::vector<std::string>& v, std::regex ext_pattern);
    extern uint8_t *readFile(const char *filename, uint32_t *p_size);
    bool writeFile(const char *filename, uint8_t *buf, uint32_t size);
    std::string getFileExtension(const std::string& s);
}

#endif /* helper_functions_hpp */
