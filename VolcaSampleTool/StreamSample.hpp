//
//  StreamSample.hpp
//  VolcaSampleTool
//
//  Created by Matthew on 22/09/2018.
//  Copyright © 2018 Matthew. All rights reserved.
//

#ifndef StreamSample_hpp
#define StreamSample_hpp

#include <stdio.h>
#include "maximilian.h"

class StreamSample : public maxiSample
{
public:
    bool readStream(short* stream);
};

#endif /* StreamSample_hpp */
