//
//  Variant.h
//  MikrotikManager
//
//  Created by Gustavo Campos on 6/12/14.
//  Copyright (c) 2014 Gustavo Campos. All rights reserved.
//

#ifndef __MikrotikManager__Variant__
#define __MikrotikManager__Variant__

#include <iostream>
#include <string>
#include <Util.h>
#include <Exception.h>

class Variant
{
private:
    string strValues;
    string strReturn;

    uint32_t nCounter;

    string strError;

    bool VariableFinder (const char* pszField, string& strWork, char chDelimiter, char chDataDv);

protected:
    
public:
    
    Variant ();
    ~Variant ();
    
    Variant& operator= (const string& strData);

    const char* operator[] (const char* pszVariable);
    
    int32_t GetInteger (const char* pszVariable);
    float   GetFloat (const char* pszVariable);
    

    bool IsAvailable (const char* pszVariable);
    
    uint32_t GetCounter ();
};

#endif /* defined(__MikrotikManager__Variant__) */
