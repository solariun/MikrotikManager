//
//  Variant.cpp
//  MikrotikManager
//
//  Created by Gustavo Campos on 6/12/14.
//  Copyright (c) 2014 Gustavo Campos. All rights reserved.
//

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


