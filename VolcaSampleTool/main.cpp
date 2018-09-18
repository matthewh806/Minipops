//
//  main.cpp
//  VolcaSampleTool
//
//  Created by Matthew on 18/09/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#include <iostream>
#include "korg_syro_volcasample.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    SyroData syroData[10];
    SyroStatus status;
    SyroHandle handle;
    uint32_t frame;
    
    status = SyroVolcaSample_Start(&handle, syroData, 0, 0, &frame);
    
    std::cout << status << std::endl;
    
    
    return 0;
}
