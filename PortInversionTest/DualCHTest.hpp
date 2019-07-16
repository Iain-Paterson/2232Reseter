//
//  DualCHTest.hpp
//  PortInversionTest
//
//  Created by Iain Paterson on 05/07/2019.
//  Copyright © 2019 Iain Paterson. All rights reserved.
//

#ifndef DualCHTest_hpp
#define DualCHTest_hpp

#include <string>
#include <stdio.h>
#include "../ftd2xx.h"

using namespace std;
class DualCHTest;

FT_STATUS DualCHSerialTest ( DualCHTest * dt );

class DualCHTest
{
public:
    DualCHTest ( void );
    ~DualCHTest ( void );

private:
    bool mRunTest;
    thread mThread;
    
    FT_STATUS InitSerialTest();

public:
    FT_STATUS start ( void );
    FT_STATUS stop ( void );
};

#endif /* DualCHTest_hpp */
