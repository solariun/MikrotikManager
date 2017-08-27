//
//  whois.h
//  MikrotikManager
//
//  Created by Gustavo Campos on 08/07/14.
//  Copyright (c) 2014 Gustavo Campos. All rights reserved.
//
/*
 
 MIT License
 
 Copyright (c) 2017 Luiz Gustavo de Campos
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
*/

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


