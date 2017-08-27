//
//  whois.h
//  MikrotikManager
//
//  Created by Gustavo Campos on 08/07/14.
//  Copyright (c) 2014 Gustavo Campos. All rights reserved.
//

#ifndef __MikrotikManager__whois__
#define __MikrotikManager__whois__

#include <Exception.h>
#include <iostream>
#include <Socket.h>
#include <Variant.h>
#include <stdio.h>
#include <ctype.h>
#include <map>
#include <string>
#include <algorithm>
#include <time.h>

#endif /* defined(__MikrotikManager__whois__) */


struct WhoisCache
{
    map<string,string> mapWhoisFields;
    time_t tEntrytime;
};



#define WHOIS_DEFAULT  "whois.iana.org"

class whois : Exception
{
protected:

    bool ProcessWhois (const char* pszHost, const char* pszLookup);

    map<string,string> mapWhoisFields;

    void ProcessCache (const char* pszValue, uint16_t nStrLen, map<string,string>& mapWhoisFields);

private:
    Socket  objSocket;
    string strError;

    uint32_t nCacheTimeout;

public:

    whois ();
    ~whois ();

    bool Lookup (const char* pszValue, uint16_t nStrLen);

    const char* operator[] (const char* pszVariable);

    void SetCacheTimeout (uint32_t nCacheTimeout);

    void IfException ();

    static uint32_t GetCacheSize ();

};


