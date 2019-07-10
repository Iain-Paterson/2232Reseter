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
#include <thread>
#include <chrono>
#include <tuple>
#include "../ftd2xx.h"

using namespace std;

void RunResetTest( FT_HANDLE ftHandle,  chrono::seconds period, chrono::milliseconds dwell);

const UCHAR RESET_MODULE_MASK = 0xFE; // mask to pull down reset pin DO
static atomic<bool> runFlag{true};

class DeviceReseter
{
private:
    FT_STATUS    mFtStatus = FT_OK;
    FT_HANDLE    mFtHandle;
    chrono::seconds mResetPeriod; // the reset period in seconds.
    chrono::milliseconds mResetDwell;  // the duration the pin is pulled low in milli seconds.
    int mPortNumber;
    shared_ptr<std::thread> mT;
    

public:
    
    DeviceReseter();
    ~DeviceReseter();
    //DeviceReseter( int resetPeriod, int resetDwell  );
    DeviceReseter( chrono::seconds resetPeriod, chrono::milliseconds resetDwell );
    std::tuple<FT_STATUS, std::shared_ptr<std::thread>> start(void);
    FT_STATUS stop(void);
private:
    
    FT_STATUS InitBitmodeBySerialNo ( void );
    
};


#endif /* DeviceReseter_hpp */
