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
#include <vector>

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
    
    static uint16_t get16BitValue(uint8_t *ptr) 
    {
        uint16_t dat;

        // TODO: wtf is going on here
        dat = (uint16_t)ptr[1];
        dat <<= 8;
        dat |= (uint16_t)ptr[0];

        return dat;
    }

    static uint32_t get32BitValue(uint8_t *ptr)
    {
        int i;
        uint32_t dat;

        dat = 0;

        for(i=0; i < 4; i++) 
        {
            dat <<= 8;
            dat |= (uint32_t)ptr[3-i];
        }

        return dat;
    }

    static void set32BitValue(uint8_t *ptr, uint32_t dat)
    {
        // TODO: Don't you dare delete this comment until you understand this code!!
        for(int i = 0; i < 4; i++)
        {
            *ptr++ = (uint8_t)dat;
            dat >>= 8;
        }
    }
    
    void readDirectory(const char *dirname, std::vector<std::string>& v)
    {
        DIR* dirp = opendir(dirname);
        struct dirent *dp;
        while ((dp = readdir(dirp)) != NULL) {
            v.push_back(dp->d_name);
        }
        closedir(dirp);
    }

    static uint8_t *readFile(const char *filename, uint32_t *p_size)
    {
        FILE *fp;
        uint8_t *buf;
        uint32_t size;

        fp = fopen(filename, "rb");
        if(!fp) {
            printf(" File open error, %s \n", filename);
            return NULL;
        }
         
        fseek(fp, 0, SEEK_END);
	    size = ftell(fp);
	    fseek(fp, 0, SEEK_SET);
        
        buf = (uint8_t*)malloc(size);
	    if (!buf) {
		    printf (" Not enough memory for read file.\n");
		    fclose(fp);
		    return NULL;
	    }

	    if (fread(buf, 1, size, fp) < size) {
		    printf (" File read error, %s \n", filename);
		    fclose(fp);
		    free(buf);
		    return NULL;
	    }

	    fclose(fp);

	    *p_size = size;
	    return buf;
    }
    
    static bool writeFile(const char *filename, uint8_t *buf, uint32_t size)
    {
        FILE *fp;
        
        fp = fopen(filename, "wb");
        if(!fp) {
            printf(" File open error, %s \n", filename);
            return false;
        }
        
        if(fwrite(buf, 1, size, fp) < size) {
            printf(" File write error, %s \n", filename);
            fclose(fp);
            return false;
        }
        
        fclose(fp);
        return true;
    }
}

#endif /* helper_functions_hpp */
