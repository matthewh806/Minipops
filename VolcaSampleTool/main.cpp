//
//  main.cpp
//  VolcaSampleTool
//
//  Created by Matthew on 18/09/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#include <iostream>
#include "korg_syro_volcasample.h"

#define WAV_POS_RIFF_SIZE 0x04
#define WAV_POS_DATA_SIZE 0x28

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

static void set32BitValue(uint8_t *ptr, uint32_t dat)
{
    // TODO: Don't you dare delete this comment until you understand this code!!
    for(int i = 0; i < 4; i++)
    {
        *ptr++ = (uint8_t)dat;
        dat >>= 8;
    }
}

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

int main(int argc, const char * argv[]) {
    
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
    
    if(argc != 2) {
        printf(" Syntax: >%s output.wav \n", argv[0]);
        return 1;
    }
    
    constructDeleteData(syroData);
    
    status = SyroVolcaSample_Start(&handle, syroData, numOfData, 0, &frame);
    if(status != Status_Success) {
        printf(" Start error, %d \n", status);
        freeSyroData(syroData, numOfData);
        
        return 1;
    }
    
    size_dest = (frame * 4) + sizeof(wav_header); // TOOD: why x 4?
    
    buf_dest = (uint8_t*) malloc(size_dest);
    if(!buf_dest) {
        printf(" Not enough memory for wtite file.\n");
        SyroVolcaSample_End(handle);
        freeSyroData(syroData, numOfData);

        return 1;
    }

    memcpy(buf_dest, wav_header, sizeof(wav_header));
    set32BitValue((buf_dest + WAV_POS_RIFF_SIZE), (frame * 4 + 0x24));
    set32BitValue((buf_dest + WAV_POS_DATA_SIZE), (frame * 4));
    
    write_pos = sizeof(wav_header);
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
    
    if(writeFile(argv[1], buf_dest, size_dest))
        printf("Conversion complete\n");
    
    free(buf_dest);
    
    return 0;
}
