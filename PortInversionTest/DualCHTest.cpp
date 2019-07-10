//
//  DualCHTest.cpp
//  PortInversionTest
//
//  Created by Iain Paterson on 05/07/2019.
//  Copyright © 2019 Iain Paterson. All rights reserved.
//
// WHen instantiated and run this class performas a test between two channels on
// a FT2232H withe the channels connected with a null modem.
// the objective of the test was to detect the 'port inverson' defect reported by Apple.

#include <iostream>

#include "DualCHTest.hpp"

DualCHTest::DualCHTest ( void )
{
    std::cout << "Constructing DualTest\n";
    
}


DualCHTest::~DualCHTest ( void )
{
    std::cout << "Destructing DualTest\n";
}


