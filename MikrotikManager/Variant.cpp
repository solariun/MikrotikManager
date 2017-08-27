//
//  Variant.cpp
//  MikrotikManager
//
//  Created by Gustavo Campos on 6/12/14.
//  Copyright (c) 2014 Gustavo Campos. All rights reserved.
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

#include "Variant.h"



Variant::Variant ()
{
    return;
}



Variant::~Variant ()
{
    return;
}



Variant& Variant::operator=(const string &strData)
{
    this->strValues = strData;

    //Counting variables
    nCounter  = 0;
    bool bText = false;
    char chChar;

    for (uint32_t nCount=0; nCount < strData.length(); nCount++)
    {
        chChar = strData.c_str() [nCount];

        if (chChar == '=' && bText == false)
        {
            nCounter++;
            bText = true;
        }
        else if (chChar == ';' && bText == true)
        {
            bText = false;
        }
    }

    //Returning;
    return *this;
}



bool Variant::IsAvailable (const char* pszVariable)
{
    return VariableFinder (pszVariable, this->strReturn, ';', '=');
}



const char* Variant::operator[] (const char* pszVariable)
{
    strError = ""; strError = strError +  "Variable [" + pszVariable + "] does not exist.";

    CHECK (VariableFinder (pszVariable, this->strReturn, ';', '=') == true, strError.c_str());
    
    return strReturn.c_str();
}



int32_t Variant::GetInteger (const char* pszVariable)
{
    int32_t nValue;
    const char* pszValue = (*this) [pszVariable];
    
    nValue = strtod (pszValue, NULL);
    
    return nValue;
}



float Variant::GetFloat (const char* pszVariable)
{
    float nValue;
    const char* pszValue = (*this) [pszVariable];

    nValue = strtof (pszValue, NULL);

    return nValue;
}



bool Variant::VariableFinder (const char* pszField, string& strWork, char chDelimiter, char chDataDv)
{
    int    nCount;
    //char   pszTmpField [UTIL_FIELDLEN + 1];
    int    nType;
    bool   bMatch;
    
    const char* pszSource = this->strValues.c_str();
    uint32_t nSourceLen = (uint32_t) this->strValues.length();
    uint nFildOffSet = 0;
    
    nType  = 0;
    nCount = 0;
    
    bMatch = true;
    
    strWork.clear();
    
    for (nCount = 0; nCount < nSourceLen || *pszSource != '\0'; nCount++)
    {
        if (*pszSource != chDelimiter)
        {
            if (nType == 0)
            {
                if (*pszSource != chDataDv && bMatch == true)
                {
                    if (*pszSource == pszField [nFildOffSet])
                    {
                        nFildOffSet++;
                        bMatch = true;
                    }
                    else
                    {
                        nFildOffSet = 0;
                        bMatch = false;
                    }
                }
                else if (bMatch == true && *pszSource == chDataDv)
                {
                    nType  = 1;
                }

            }
            else if (nType == 1)
            {
                strWork.push_back(*pszSource);
                //pszData [nCount] = *pszSource;
            }
        }
        else if (bMatch == true && nFildOffSet == strlen (pszField))
        {
            return true;
        }
        else
        {
            strWork.clear();
            nFildOffSet = 0;
            nType  = 0;
            bMatch = true;
        }
        
        pszSource++;
    }
    
    return false;
}


uint32_t Variant::GetCounter ()
{
    return nCounter;
}


