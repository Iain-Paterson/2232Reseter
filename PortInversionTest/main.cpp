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

int Boom (int argc)
{
    int *array = new int[100];
    delete [] array;
    return array[argc];  // BOOM
}

int main(int argc, const char * argv[]) {
    //char  burl[] = { '/','-','\\','|','/','-','\\','|'};
    //Boom(argc);
    cout << "Hello, World!\n";
    {
        shared_ptr<DeviceReseter> dr = make_shared<DeviceReseter>(chrono::seconds(90), chrono::milliseconds(100) );
        shared_ptr<DualCHTest>    dut= make_shared<DualCHTest>();
    
        dut->start();
        auto [r,t] = dr->start();
        thread gkt(GetKeyboardTerminate);
        while(run)
        {
            this_thread::sleep_for( chrono::milliseconds(100) );
        }
        dut->stop();
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
