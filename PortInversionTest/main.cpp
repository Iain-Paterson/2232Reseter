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

void GetKeyboardTerminate( void );

using namespace std;
atomic<bool> run { true };

int main(int argc, const char * argv[]) {
    //char  burl[] = { '/','-','\\','|','/','-','\\','|'};

    shared_ptr<thread> t1;
    
    
    cout << "Hello, World!\n";
    {
        shared_ptr<DeviceReseter> dr = make_shared<DeviceReseter>(chrono::seconds(10),chrono::milliseconds( 100) );
        shared_ptr<DualCHTest>    dut= make_shared<DualCHTest>();
        auto [r,t] = dr->start();
        t1 = t;
        //int c=0;
        cout << "*";
        thread gkt(GetKeyboardTerminate);
        while(run)
        {
        
            this_thread::sleep_for( chrono::milliseconds(100) );
            //cout << "\b\b" << burl[c++ % 8];
            //cout << ".";
            //if ( getchar() == 'q')
            //    run = false;
            
        }
        dr->stop();
    
        cout << "bye.\n";
        gkt.join();
    }
    cout << "caio!\n";
    
    return 0;
}

void GetKeyboardTerminate( void )
{
    cout << "GetKeyboardTerminate+++\n";
    cout << "quit?\n";
    while(run)
    {
        if ( getchar() == 'q')
            run = false;
    }
    cout << "GetKeyboardTerminate---\n";
    return;
}
