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



class DualCHTest
{
public:
    DualCHTest ( void );
    ~DualCHTest ( void );
private:
    bool mStartTest;
    std::string mTestPortDescriptor;
    
    
    void StartSerialTest ( std::string descripter);
};

#endif /* DualCHTest_hpp */
