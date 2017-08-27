//
//  main.h
//  MikrotikManager
//
//  Created by Gustavo Campos on 09/06/14.
//  Copyright (c) 2014 Gustavo Campos. All rights reserved.
//

#ifndef MikrotikManager_main_h
#define MikrotikManager_main_h

#include <Util.h>
#include <map>
#include <vector>

struct
{
    uint nTXLimit;
    uint nRXLimit;

    const char* pszInterface;
    const char* pszRouterAddress;

    uint  nRouterCPULoad;
    uint  nCPUCounting;
    uint  nCPUFrequence;

    string strCPU;
    string strUpTime;
    string strRouterVersion;
    uint32_t  nTotalMemory;
    uint32_t  nFreeMemory;
    string strArchitecture;
    string strBoardName;
    string strPlatform;

    uint32_t   nFreeHDDSpace;
    uint32_t   nTotalHDDSpace;

    uint32_t nCPUVoltage;
    uint32_t nAmbienteTemp;
    uint32_t nCPUTemp;
    bool     bHealthSupport;

} ServerAttr;

struct MktProfile
{
    string strType;
    uint32_t  nUsage;
};


struct Interfaces
{
    string strName;
    string strType;
    string strRunning;
    string strDisable;

    uint32_t nTX_bits_seconds;
    uint32_t nTX_drops_seconds;
    uint32_t nTX_error_seconds;

    uint32_t nRX_bits_seconds;
    uint32_t nRX_drops_seconds;
    uint32_t nRX_error_seconds;
};


/* Dual layer Monitoring Structure */
struct IPConnectionMonitor
{
    string szDstAddress;
    uint32_t nTX;
    uint32_t nTXPacket;
    uint32_t nRX;
    uint32_t nRXPacket;
};

struct IPMonitoring
{
    string szSrcAddress;

    uint32_t nTX;
    uint32_t nTXPacket;

    uint32_t nRX;
    uint32_t nRXPacket;

    vector <struct IPConnectionMonitor> vecConnections;
};



struct IPUsage
{
    char        szSrcAddress [20];
    char        szDstAddress [20];
    uint32_t    nRXbps;
    uint32_t    nTXbps;
    uint32_t    nTXPackets;
    uint32_t    nRXPackets;
    char        szMacAddress [20];
    char        szHostName [50];

    uint32_t    nCountConnections;

    vector <struct IPConnectionMonitor> pvecConnections;
};



#endif
