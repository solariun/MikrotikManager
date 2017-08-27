//
//  Variant.h
//  MikrotikManager
//
//  Created by Gustavo Campos on 6/12/14.
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
