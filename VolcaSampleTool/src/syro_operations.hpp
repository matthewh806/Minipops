//
//  syro_operations.hpp
//  VolcaSampleTool
//
//  Created by Matthew on 02/10/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#ifndef syro_operations_hpp
#define syro_operations_hpp

#include <stdio.h>
#include <vector>
#include "korg_syro_volcasample.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace syro_operations {
    extern void freeSyroData(SyroData *syro_data, int num_of_data);
    extern void constructDeleteData(SyroData *syro_data, int number);
    extern void constructAddData(SyroData *syro_data, const char *filename, int number);
    extern int constructSyroStream(const char *out_filename, const char *in_files, SyroDataType data_type, std::vector<int> slots);
    extern bool setupSampleFile(const char *filename, SyroData *syroData);
}

#endif /* syro_operations_hpp */
