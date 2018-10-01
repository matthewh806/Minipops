//
//  syro_operations.cpp
//  VolcaSampleTool
//
//  Created by Matthew on 02/10/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#include "syro_operations.hpp"
#include <iostream>
#include "helper_functions.hpp"

namespace syro_operations {
    void freeSyroData(SyroData *syro_data, int num_of_data)
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
    
    void constructDeleteData(SyroData *syro_data, int number)
    {
        syro_data->DataType = DataType_Sample_Erase;
        syro_data->pData = NULL;
        syro_data->Number = number;
        syro_data->Size = 0;
        
        syro_data++;
    }
    
    int constructSyroStream(const char *out_filename, const char *in_files, SyroDataType data_type, std::vector<int> slots)
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
        
        auto count = slots.size();
        
        std::cout << " operating on " << count << " slots" << std::endl;
        
        // in_files is only valid for adding not deleting
        for(int i = 0; i < count; i++) {
            switch (data_type) {
                case DataType_Sample_Erase:
                    std::cout << "constructing delete data for slot: " << slots[i] << std::endl;
                    syro_operations::constructDeleteData(syroData, slots[i]);
                    std::cout << "goodbye little sample :'(" << std::endl;
                    break;
                case DataType_Sample_Compress:
                    std::cout << "constructing add data for slot: " << slots[i] << std::endl;
                    syro_operations::constructAddData(syroData, in_files, slots[i]);
                    break;
                default:
                    break;
            }
        }
        
        status = SyroVolcaSample_Start(&handle, syroData, numOfData, 0, &frame);
        if(status != Status_Success) {
            printf(" Start error, %d \n", status);
            syro_operations::freeSyroData(syroData, numOfData);
            
            return 1;
        }
        
        size_dest = (frame * 4) + sizeof(volca_constants::wav_header); // TOOD: why x 4?
        
        buf_dest = (uint8_t*) malloc(size_dest);
        if(!buf_dest) {
            printf(" Not enough memory for write file.\n");
            SyroVolcaSample_End(handle);
            syro_operations::freeSyroData(syroData, numOfData);
            
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
        syro_operations::freeSyroData(syroData, numOfData);
        
        if(volca_helper_functions::writeFile(out_filename, buf_dest, size_dest))
            printf("Conversion complete\n");
        
        free(buf_dest);
        
        return 0;
    }
    
    bool setupSampleFile(const char *filename, SyroData *syroData)
    {
        uint8_t *src;
        uint32_t wav_pos, size;
        uint32_t chunk_size;
        uint32_t wav_fs;
        uint16_t num_of_ch, sample_byte;
        uint32_t num_of_frame;
        
        src = volca_helper_functions::readFile(filename, &size);
        if(!src) {
            return false;
        }
        
        if(size <= sizeof(volca_constants::wav_header)) {
            printf("wav file error, too small. \n");
            free(src);
            return false;
        }
        
        // check header, fmt
        if(memcmp(src, volca_constants::wav_header, 4)) {
            printf("wav file error, 'RIFF' is not found.\n");
            free(src);
            return false;
        }
        
        if (memcmp((src + WAV_POS_WAVEFMT), (volca_constants::wav_header + WAV_POS_WAVEFMT), 8)) {
            printf ("wav file error, 'WAVE' or 'fmt ' is not found.\n");
            free(src);
            return false;
        }
        
        wav_pos = WAV_POS_WAVEFMT + 4;
        
        if(volca_helper_functions::get16BitValue(src + wav_pos + 8 + WAVFMT_POS_ENCODE) != 1) {
            printf("wav file error, encode must be '1'. \n");
            free(src);
            return false;
        }
        
        num_of_ch = volca_helper_functions::get16BitValue(src + wav_pos + 8 + WAVFMT_POS_CHANNEL);
        if((num_of_ch != 1) && (num_of_ch !=2)) {
            printf("wav file error, channel must be 1 or 2.\n");
            free(src);
            return false;
        }
        
        {
            uint16_t num_of_bit;
            
            num_of_bit = volca_helper_functions::get16BitValue(src + wav_pos + 8 + WAVFMT_POS_BIT);
            if((num_of_bit != 16) && (num_of_bit != 24)) {
                printf("wav file error, bit must be 16 or 24.\n");
                free(src);
                return false;
            }
            
            sample_byte = (num_of_bit / 8);
        }
        
        wav_fs = volca_helper_functions::get32BitValue(src + wav_pos + 8 + WAVFMT_POS_FS);
        
        // search for data
        for(;;) {
            chunk_size = volca_helper_functions::get32BitValue(src + wav_pos + 4);
            if(!memcmp((src+wav_pos), "data", 4)) {
                break;
            }
            
            wav_pos += chunk_size + 8;
            if((wav_pos + 8) > size) {
                printf("wav file error, 'data' chunk not found. \n");
                free(src);
                return false;
            }
        }
        
        if((wav_pos + chunk_size + 8) > size) {
            printf("wav file error, illegal 'data' chunk size. \n");
            free(src);
            return false;
        }
        
        num_of_frame = chunk_size / (num_of_ch * sample_byte);
        chunk_size = (num_of_frame * 2); // I think this is to convert to 16bit (2*2byte) 1 ch
        syroData->pData = (uint8_t *)malloc(chunk_size);
        if(!syroData->pData) {
            printf("Not enough memory to setup file. \n");
            free(src);
            return false;
        }
        
        {
            uint8_t *poss;
            int16_t *posd;
            int32_t dat, datf;
            uint16_t ch, sbyte;
            
            poss = (src + wav_pos + 8); // src + wav_pos should point to 'd' now -> so + 8 skips over 'd','a','t','a' and data size block
            posd = (int16_t *)syroData->pData;
            
            // Convert to 1 channel, 16Bit
            for(;;) {
                datf = 0;
                
                for(ch=0; ch < num_of_ch; ch++) {
                    dat = ((int8_t *)poss)[sample_byte-1];
                    for(sbyte=1; sbyte < sample_byte; sbyte++) {
                        dat <<= 8;
                        dat |= poss[sample_byte - 1 - sbyte];
                    }
                    
                    poss += sample_byte;
                    datf += dat;
                }
                
                datf /= num_of_ch;
                *posd++ = (int16_t)datf;
                
                if(!(--num_of_frame))
                    break;
            }
        }
        
        syroData->Size = chunk_size;
        syroData->Fs = wav_fs;
        syroData->SampleEndian = LittleEndian;
        
        free(src);
        
        printf("Prepared sample ok!\n");
        
        return true;
    }
    
    void constructAddData(SyroData *syro_data, const char *filename, int number)
    {
        std::cout << "is directory: " << volca_helper_functions::isDirectory(filename) << std::endl;
        std::cout << "is file: " << volca_helper_functions::isRegularFile(filename) << std::endl;
        
        std::vector<std::string> files; // TODO: Check for wav type...
        
        if(volca_helper_functions::isDirectory(filename)) {
            volca_helper_functions::readDirectory(filename, files);
        } else if (volca_helper_functions::isRegularFile(filename)) {
            files.push_back(filename);
        } else {
            // oh dear...
        }
        
        for(int i = 0; i < files.size(); i++) {
            
            std::cout << "constructing add data for: " << files[i] << std::endl;
            
            syro_data->DataType = DataType_Sample_Compress;
            syro_data->Quality = 16;
            syro_data->Number = i;
            
            if(!setupSampleFile(files[i].c_str(), syro_data))
                continue;
            
            syro_data++;
        }
    }
}
