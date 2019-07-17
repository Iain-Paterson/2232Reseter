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
#include <chrono>
#include <thread>

#include <err.h>

#include "DualCHTest.hpp"

using namespace std;

static void CloseBothHandles( FT_HANDLE fthandle[] );
static bool TestPortOrder ( FT_HANDLE fthandle[] );
static FT_STATUS ConfigPort( FT_HANDLE handle, int& onErrorRetryCount);

atomic<bool> gRunTest (true);

DualCHTest::DualCHTest ( void )
{
    std::cout << "Constructing DualTest\n";
}


DualCHTest::~DualCHTest ( void )
{
    std::cout << "Destructing DualTest\n";
}

FT_STATUS DualCHTest::start ( void )
{
    gRunTest = true;
    mThread = thread(DualCHSerialTest, this );
    return FT_OK;
}

FT_STATUS DualCHTest::stop ( void )
{
    gRunTest = false;
    mThread.join();
    return FT_OK;
}


FT_STATUS DualCHSerialTest( DualCHTest *dt )
{
    uint32_t iteration = 0;
    FT_HANDLE handleA = 0;
    FT_HANDLE handleB = 0;
    
    char TestBufferData[128];
    
    //std::vector<FT_HANDLE> ftHandles{ handleA, handleB };
    FT_HANDLE fthandles[] = {handleA, handleB};
    
    FT_STATUS ftStatus;
    DWORD numDevs;
    FT_DEVICE_LIST_INFO_NODE *devInfo;
    
    DWORD devIndex = 0;
    char buffer[64];
    
    for ( int c=0 ; c<128; c++ )
        TestBufferData[c] = 'a' +c;
    
    ftStatus = FT_ListDevices( &numDevs, NULL, FT_LIST_NUMBER_ONLY);
    if( ftStatus ==FT_OK )
    {
        cout << numDevs << " devices detected.\n";
    }
    else
    {
        cout << "No devices detected\n";
        return -1;
    }
    
    for ( int c = 0 ;c < numDevs ; c++ )
    {
        devIndex = c;
        ftStatus = FT_ListDevices((PVOID)devIndex,buffer,FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER);
        if (ftStatus == FT_OK)
        {
            // FT_ListDevices OK, serial number is in Buffer
            cout << "Dev[" << c << "]: serial no:" << buffer << "\n";
        }
        else
        {
            
        }
    }
    
    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    
    if (numDevs > 0) {
        // allocate storage for list based on numDevs
        cout << "Getting Device Info for" << numDevs << "devs\n";
        devInfo =
        (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
        if( devInfo != NULL)
        {
            cout << "devinfo structure malloc'ed at mem 0x" << hex << devInfo << "\n";
        }
        else
        {
            cout << "devinfo structure Failed to malloc\n";
        }
        // get the device information list
        ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
        printf("Get Device Info List Return Status: 0x%04x\n", ftStatus );
        if (ftStatus == FT_OK) {
            for (int i = 0; i < numDevs; i++) {
                cout << "Dev " << i << ":\n";
                cout << "  Flags=0x" << hex << devInfo[i].Flags << "\n";
                cout << "  Type=0x" << hex << devInfo[i].Type << "\n";
                cout << "  ID=0x" << hex << devInfo[i].ID << "\n";
                cout << "  LocId=0x" << hex << devInfo[i].LocId << "\n";
                cout << "  SerialNumber=" << devInfo[i].SerialNumber << "\n";
                cout << "  Description=" << devInfo[i].Description << "\n";
                cout << "  ftHandle=0x" << hex << devInfo[i].ftHandle  << "\n";
            }
        }
        else
        {
            cout << "Get Device Info failed.\n";
        }
    }
    
    
    
    
    auto onErrorRetryCount = 0;
    while (gRunTest) {
        //for( auto handle : ftHandles)
        for (int c = 0; c<2;c++)
        {
            auto handle = fthandles[c];
            
            FT_STATUS ftStatus = FT_Open(c, &handle);
            fthandles[c] = handle; //ugly :-/
            if (ftStatus != FT_OK) {
                cout << "Could not open device" << c << ":" << ftStatus << "\n";
                onErrorRetryCount++;
                break;
            }
            
            if ( FT_OK != ConfigPort( handle, onErrorRetryCount ) )
                break;
        }
        
        if ( onErrorRetryCount)
        {
            cout << "Error initialising, closing handles and retrying after two shakes: " << dec << onErrorRetryCount << "\n";
            CloseBothHandles( fthandles );
            this_thread::sleep_for(chrono::seconds(15));
            continue;
        }
        
        if ( !TestPortOrder (fthandles) )
        {
            cout << "Port Order Error!\n";
            continue;
        }

        unsigned int bytesWritten = 0;
        ftStatus = FT_Write(fthandles[1], &TestBufferData, sizeof(TestBufferData),
                            &bytesWritten);
        if (ftStatus != FT_OK || bytesWritten != sizeof(TestBufferData)) {
            cout << "Could not write data: status:" << ftStatus << " bytesWritten: " << bytesWritten << "\n";
            CloseBothHandles( fthandles );
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }
        else
        {
            // cout << "wrote" << bytesWritten << " bytes\n";
        }
        
        unsigned int rxQueue;
        unsigned int attempts = 0;
        
        for ( int c=0 ;c < 2 ; c++ ){
            ftStatus = FT_GetQueueStatus(fthandles[c], &rxQueue);
            if ( ftStatus != FT_OK)
                cout << "Get Queue Status failed device : " << c << " with status " << ftStatus <<"\n";
           // else
           //     cout << "device[" << c << "] queue status: " << rxQueue << "\n";
        }
        
        while (1) {
            ftStatus = FT_GetQueueStatus(fthandles[0], &rxQueue);
            if (ftStatus != FT_OK) {
                onErrorRetryCount++;
                CloseBothHandles( fthandles );
                cout << "Failed to read queue status.\n";
                break;
            }
            if (rxQueue == 0) {
                attempts += 1;
                if (attempts > 10000) {
                    onErrorRetryCount++;
                    CloseBothHandles( fthandles );
                    cout << "Nothing received. Timeout.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }
                //usleep(1);
                this_thread::sleep_for( chrono::microseconds(1) );
                
            }
            else {
                break;
            }
        }
        if (onErrorRetryCount)
            continue;
        
        unsigned char receiveBuffer[128];
        unsigned int bytesReceived = 0;
        ftStatus = FT_Read(fthandles[0], receiveBuffer, sizeof(receiveBuffer),
                           &bytesReceived);
        if (ftStatus != FT_OK) {
            const char* buffer = "This test data is on port 0.\n";
            for ( int c = 0 ; c< 100 ; c++)
            {
                if ( (ftStatus = FT_Write  (fthandles[0], (void*)buffer, sizeof(buffer), &bytesWritten)) == FT_IO_ERROR)
                    cout << "+##";
                this_thread::sleep_for( chrono::milliseconds(500));
            }
            cout << "Could not read data: " << ftStatus << "\n";
             CloseBothHandles(fthandles);
             this_thread::sleep_for(chrono::seconds(2));
        }
        else if (bytesReceived == 0) {
            CloseBothHandles(fthandles);
            cout << "received 0 bytes\n";
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }
        else {
            /*printf("received %u bytes: ", bytesReceived);
            for (int i = 0; i < bytesReceived; ++i) {
                printf(" %02x", receiveBuffer[i]);
            }
            printf("\n");*/
            cout << ".";
        }
        //for( auto handle : ftHandles)
    
        CloseBothHandles(fthandles);
    
        iteration++;
    }
    return FT_OK;
}

void CloseBothHandles( FT_HANDLE fthandle[] )
{
    for( int c=0; c<2;c++)
    {
        auto handle = fthandle[c];
        FT_Close(handle);
    }
}


bool TestPortOrder ( FT_HANDLE fthandle[] )
{
    bool retVAl = true;
    FT_DEVICE ftDeviceA;
    DWORD deviceIDA;
    char SerialNumberA[16];
    char DescriptionA[64];
    
    FT_DEVICE ftDeviceB ;
    DWORD deviceIDB;
    char SerialNumberB[16];
    char DescriptionB[64];
    
    if (FT_OK == FT_GetDeviceInfo(
                                fthandle[0],
                                &ftDeviceA,
                                &deviceIDA,
                                SerialNumberA,
                                DescriptionA,
                                NULL
                                )
        &&
        FT_OK == FT_GetDeviceInfo(
                                  fthandle[1],
                                  &ftDeviceB,
                                  &deviceIDB,
                                  SerialNumberB,
                                  DescriptionB,
                                  NULL
                                  )
        )
    {
        if( strcmp ( SerialNumberA , "FT42Q1VFA") || strcmp ( SerialNumberB , "FT42Q1VFB"))
        {
            cout << "Port Order Error!\n";
            retVAl = false;
            
        }

    }
    return retVAl;
}


FT_STATUS ConfigPort( FT_HANDLE handle, int& onErrorRetryCount)
{
    FT_STATUS ftStatus = FT_IO_ERROR;
    // Set RTS/CTS flow control
    ftStatus = FT_SetFlowControl(handle, FT_FLOW_RTS_CTS, 0x11, 0x13);
    if (ftStatus == FT_OK)
    {
        // FT_SetFlowControl OK
        //cout << "flow control set.\n";
    }
    else {
        cout << "Error flow control not set.\n";
        onErrorRetryCount++;
        return ftStatus;
    }
    // FT_SetFlowControl Failed
    // Latency Timer
    ftStatus = FT_SetLatencyTimer(handle, 5);
    if (ftStatus != FT_OK) {
        cout << "Could not set latency timer: " << ftStatus;
        onErrorRetryCount++;
        return ftStatus;
    }
    
    // Baud Rate
    ftStatus = FT_SetBaudRate(handle, 115200);
    if (ftStatus != FT_OK) {
        cout <<"Could not set baud rate: "<<  ftStatus;
        onErrorRetryCount++;
        return ftStatus;
    }
    
    // Set timeouts
    ftStatus = FT_SetTimeouts(handle, 1000, 1000);
    if (ftStatus != FT_OK) {
        cout << "Could not set timeouts: " << ftStatus;
        onErrorRetryCount++;
        return ftStatus;
    }
    
    // Clear buffers
    ftStatus = FT_Purge(handle, FT_PURGE_RX | FT_PURGE_TX);
    if (ftStatus != FT_OK) {
        //errx(1, "Could not purge buffers: %d", ftStatus);
        cout << "Could not purge buffers: " << ftStatus << "\n";
        onErrorRetryCount++;
        return ftStatus;
    }
    else
    {
        onErrorRetryCount = 0;
    }
    
    return ftStatus;
}

