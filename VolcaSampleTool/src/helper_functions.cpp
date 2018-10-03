//
//  helper_functions.cpp
//  VolcaSampleTool
//
//  Created by Matthew on 02/10/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#include <stdio.h>
#include "helper_functions.hpp"

namespace volca_helper_functions {
    
    auto console = spdlog::stdout_color_mt("helper_functions");
    
    uint16_t get16BitValue(uint8_t *ptr)
    {
        uint16_t dat;
        
        dat = (uint16_t)ptr[1];
        dat <<= 8;
        dat |= (uint16_t)ptr[0];
        
        return dat;
    }
    
    uint32_t get32BitValue(uint8_t *ptr)
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
    
    void set32BitValue(uint8_t *ptr, uint32_t dat)
    {
        for(int i = 0; i < 4; i++)
        {
            *ptr++ = (uint8_t)dat;
            dat >>= 8;
        }
    }
    
    bool isRegularFile(const char *path)
    {
        struct stat path_stat;
        stat(path, &path_stat);
        
        return S_ISREG(path_stat.st_mode);
    }
    
    bool isDirectory(const char *path)
    {
        struct stat path_stat;
        stat(path, &path_stat);
        
        return S_ISDIR(path_stat.st_mode);
    }
    
    std::string getFileExtension(const std::string& s)
    {
        size_t i = s.rfind('.', s.length());
        if (i != std::string::npos) {
            return(s.substr(i+1, s.length() - i));
        }
        
        return "";
    }
    
    
    void readDirectory(const char *dirname, std::vector<std::string>& v, std::regex ext_pattern)
    {
        // NOTE: Includes dir in path name vector out
        DIR* dirp = opendir(dirname);
        struct dirent *dp;
        while ((dp = readdir(dirp)) != NULL) {
            // TODO: THis doesn't seem to work, but it would be nice.
//            if(!isRegularFile(dp->d_name)) {
//                console->debug("{} not a regular file, skipping", dp->d_name);
//                continue;
//            }
//
            const std::string extension = getFileExtension(dp->d_name);
            if(!std::regex_match(extension, ext_pattern))
            {
                console->debug("{} (ext {}). Doesn't have expected extenson, skipping", dp->d_name, extension);
                continue;
            }
            
            char path[100];
            
            strcpy(path, dirname);
            strcat(path, dp->d_name);
            v.push_back(path);
        }
        closedir(dirp);
    }
    
    uint8_t *readFile(const char *filename, uint32_t *p_size)
    {
        FILE *fp;
        uint8_t *buf;
        uint32_t size;
        
        fp = fopen(filename, "rb");
        if(!fp) {
            console->error("File open error {}", filename);
            return NULL;
        }
        
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        buf = (uint8_t*)malloc(size);
        if (!buf) {
            console->error("Not enough memory for read file", filename);
            fclose(fp);
            return NULL;
        }
        
        if (fread(buf, 1, size, fp) < size) {
            console->error("File read error {}", filename);
            fclose(fp);
            free(buf);
            return NULL;
        }
        
        fclose(fp);
        
        *p_size = size;
        return buf;
    }
    
    bool writeFile(const char *filename, uint8_t *buf, uint32_t size)
    {
        FILE *fp;
        
        fp = fopen(filename, "wb");
        if(!fp) {
            console->error("File open error {}", filename);
            return false;
        }
        
        if(fwrite(buf, 1, size, fp) < size) {
            console->error("File write error {}", filename);
            fclose(fp);
            return false;
        }
        
        fclose(fp);
        return true;
    }
}
