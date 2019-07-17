//
//  DeviceReseter.cpp
//  PortInversionTest
//
//  Created by Iain Paterson on 05/07/2019.
//  Copyright © 2019 Iain Paterson. All rights reserved.
//
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <assert.h>
#include "DeviceReseter.hpp"


DeviceReseter::DeviceReseter()
{
    DeviceReseter( chrono::seconds(45), chrono::milliseconds(30) ); // default initialisation to 10 second reset period with a reset pulse of 30ms
}

DeviceReseter::DeviceReseter( chrono::seconds resetPeriod, chrono::milliseconds resetDwell  ):mResetPeriod(resetPeriod), mResetDwell(resetDwell)
{
    std::cout << "Constructing DeviceReseter with " << mResetPeriod.count() << " second reset period" << std::endl;
    InitBitmodeBySerialNo();
}

DeviceReseter::~DeviceReseter()
{
    std::cout << "Destructing DeviceReseter\nClossing Ft Device: " << mFtHandle << "\n";
    FT_Close(mFtHandle);
}


FT_STATUS DeviceReseter::stop(void)
{
    runFlag = false;
    mT->join();
    return FT_OK;
}


FT_STATUS DeviceReseter::InitBitmodeBySerialNo ( void )
{
    FT_STATUS retVal = FT_OK;
    UCHAR outputData;
    DWORD baudRate =  9600;
    DWORD bytesWritten;
    
    std::cout << "InitBitmodeByPortNO+++\n";
    //mFtStatus = FT_Open(mPortNumber, &mFtHandle);
    mFtStatus = FT_OpenEx((void *)"FT4EU2R2", FT_OPEN_BY_SERIAL_NUMBER, &mFtHandle);
    if (mFtStatus != FT_OK)
    {
        /* FT_Open can fail if the ftdi_sio module is already loaded. */
        printf("FT_Open(%d) failed (error %d).\n", mPortNumber, (int)mFtStatus);
        printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
        printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
        return 1;
    }
    
    /* Enable bit-bang mode, where 8 UART pins (RX, TX, RTS etc.) become
     * general-purpose I/O pins.
     */
    printf("Selecting asynchronous bit-bang mode.\n");
    mFtStatus = FT_SetBitMode(mFtHandle,
                             0xFF, /* sets all 8 pins as outputs */
                             FT_BITMODE_ASYNC_BITBANG);
    if (mFtStatus != FT_OK)
    {
        printf("FT_SetBitMode failed (error %d).\n", (int)mFtStatus);
        goto exit;
    }
    
    /* In bit-bang mode, setting the baud rate gives a clock rate
     * 16 times higher, e.g. baud = 9600 gives 153600 bytes per second.
     */
    printf("Setting clock rate to %d\n", baudRate * 16);
    mFtStatus = FT_SetBaudRate(mFtHandle, baudRate);
    if (mFtStatus != FT_OK)
    {
        printf("FT_SetBaudRate failed (error %d).\n", (int)mFtStatus);
        goto exit;
    }
    
    outputData = static_cast<UCHAR>(~RESET_MODULE_MASK);
    mFtStatus = FT_Write(mFtHandle, &outputData, 1, &bytesWritten);
    if (mFtStatus != FT_OK)
    {
        printf("FT_Write failed (error %d).\n", (int)mFtStatus);
        goto exit;
    }
    
    std::cout << "InitBitmodeByPortNO---\n";
    return retVal;
    
    
exit:
    retVal = FT_IO_ERROR;
    return retVal;
}

std::tuple<FT_STATUS, std::shared_ptr<std::thread>> DeviceReseter::start()
{
    FT_STATUS retVal = FT_OK;
    
    mT = std::make_shared<std::thread>(RunResetTest,  mFtHandle, mResetPeriod,mResetDwell);

    
    return {retVal, mT};
}




void RunResetTest( FT_HANDLE ftHandle,  chrono::seconds period, chrono::milliseconds dwell)
{
    UCHAR outputData;
    DWORD bytesWritten;
    FT_STATUS ftStatus;
    std::cout << "RRT++++\n";
    do{

        // De Assert the reset pin by setting it high.
        outputData = static_cast<UCHAR>(~RESET_MODULE_MASK);
        ftStatus = FT_Write(ftHandle, &outputData, 1, &bytesWritten);
        if (ftStatus != FT_OK)
        {
            printf("FT_Write failed (error %d).\n", (int)ftStatus);
            goto exit;
        }
        std::this_thread::sleep_for(period - dwell);
        ListDevices();
        outputData = RESET_MODULE_MASK;
        ftStatus = FT_Write(ftHandle, &outputData, 1, &bytesWritten);
        if (ftStatus != FT_OK)
        {
            printf("FT_Write failed (error %d).\n", (int)ftStatus);
            goto exit;
        }
        cout << "\n";for ( int c =0 ; c < 48; c++)cout << "*";cout << "\n";
        this_thread::sleep_for( dwell );
        
    }while(runFlag);
    
    
    cout << "RRT----\n";
    return;
    
exit:
    cout << "RRT----Error!\n";
    return;
    
}




FT_STATUS ListDevices ( void )
{
    DWORD numDevs;
    FT_STATUS ftStatus = FT_IO_ERROR; //default to an Error.
    char buffer[64];
    
    
    ftStatus = FT_ListDevices( &numDevs, NULL, FT_LIST_NUMBER_ONLY);
    if( ftStatus ==FT_OK )
    {
        cout << numDevs <<  " devices detected.\n";
    }
    else
    {
        cout << "No devices detected\n";
        return -1;
    }
    for ( int c = 0 ;c < numDevs ; c++ )
    {
        auto devIndex = c;
        ftStatus = FT_ListDevices((PVOID)devIndex,buffer,FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER);
        if (ftStatus == FT_OK)
        {
            // FT_ListDevices OK, serial number is in Buffer
            cout << "Dev" << c << ": serial no: " << buffer << "\n";
        }
        else
        {
            
        }
    }
    return ftStatus;
}

