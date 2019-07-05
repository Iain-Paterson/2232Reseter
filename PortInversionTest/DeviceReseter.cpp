//
//  DeviceReseter.cpp
//  PortInversionTest
//
//  Created by Iain Paterson on 05/07/2019.
//  Copyright © 2019 Iain Paterson. All rights reserved.
//
#include <iostream>
#include <assert.h>

#include "DeviceReseter.hpp"


DeviceReseter::DeviceReseter()
{
    DeviceReseter(10,30); // default initialisation to 10 second reset period with a reset pulse of 30ms
}
DeviceReseter::DeviceReseter( int resetPeriod, int resetDwell  ):mResetPeriod(resetPeriod), mResetDwell(resetDwell)
{
    std::cout << "Constructing DeviceReseter with " << mResetDwell << "second reset period" << std::endl;
    
}
bool DeviceReseter::start(void)
{
    bool retVal = false;
    assert(retVal); //todo
    return false;
}
bool DeviceReseter::stop(void)
{
    bool retVal = false;
    assert(retVal); //todo
    return false;
}
