//
//  DeviceReseter.hpp
//  PortInversionTest
//
//  Created by Iain Paterson on 05/07/2019.
//  Copyright © 2019 Iain Paterson. All rights reserved.
//

#ifndef DeviceReseter_hpp
#define DeviceReseter_hpp
#include <iostream>
#include <stdio.h>



class DeviceReseter
{
public:
    
    DeviceReseter();
    DeviceReseter( int resetPeriod, int resetDwell  );
    bool start(void);
    bool stop(void);
private:
    int mResetPeriod; // the reset period in seconds.
    int mResetDwell;  // the duration the pin is pulled low in milli seconds.
    
};

#endif /* DeviceReseter_hpp */
