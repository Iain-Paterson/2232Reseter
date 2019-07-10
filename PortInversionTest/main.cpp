//
//  main.cpp
//  PortInversionTest
//
//  Created by Iain Paterson on 04/07/2019.
//  Copyright © 2019 Iain Paterson. All rights reserved.
//

#include <iostream>
#include <thread>
#include <chrono>

#include "DeviceReseter.hpp"
#include "DualCHTest.hpp"
#include "../ftd2xx.h"

//#pragma clang diagnostic ignored "-Wc++17-extensions"

using namespace std;

int main(int argc, const char * argv[]) {
    char  burl[] = { '/','-','\\','|','/','-','\\','|'};
    atomic<bool> run { true };
    shared_ptr<thread> t1;
    
    
    cout << "Hello, World!\n";
    {
        shared_ptr<DeviceReseter> dr = make_shared<DeviceReseter>(10, 100);
        shared_ptr<DualCHTest>    dut= make_shared<DualCHTest>();
        auto [r,t] = dr->start();
        t1 = t;
        int c=0;
        cout << "quit?\n";
        while(run)
        {
        
            this_thread::sleep_for( chrono::microseconds(200) );
            cout << "\b" << burl[c++ % 8];
            //if ( getchar() == 'q')
            //    run = false;
            
        }
        dr->stop();
        
        cout << "bye.\n";
    }
    
    
    
    cout << "caio!\n";
    
    return 0;
}
