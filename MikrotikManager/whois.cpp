//
//  whois.cpp
//  MikrotikManager
//
//  Created by Gustavo Campos on 08/07/14.
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

#define DEBUG 1
#define _DEBUG 1

#include "whois.h"

//Global cache;
static map<string, struct WhoisCache> mapWhoisCache;


void whois::IfException()
{
    NOTRACE ("Whois, Error handler activated.");
}

whois::whois ()
{
    nCacheTimeout = 0;
}

whois::~whois()
{

}


bool whois::ProcessWhois (const char* pszHost, const char* pszLookup)
{
    objSocket.Close();
    char szName [1024], szValue [1024];

    mapWhoisFields.clear();

    Verify (objSocket.Connect (pszHost, 43, 5) == true, "Error  handling connection", false);

    //TRACE ("Connected...\n");

    string strData = "";

    strData = strData + pszLookup + "\n";

    objSocket.Send (strData.length(), strData.c_str());

    //TRACE ("Data sent...\n");
    static char szLine [1024 + 1];
    try
    {
        while (true)
        {
            objSocket.Gets (szLine, sizeof (szLine), 2);
            //TRACE ("Line: [%s]\n", szLine);

            if (isalpha (szLine [0]) && strchr (szLine, ':'))
            {
                //Do not take this line, it exists for secure reasons
                szLine [strlen (szLine)] = '\0';

                //TRACE ("\n\n %s \n", szLine);

                TUtil_GetToken (':', szLine, szName, sizeof (szName), 0);
                TUtil_GetToken (':', szLine, szValue, sizeof (szValue), 1);

                TUtil_TrimString (szName, sizeof (szName));
                TUtil_TrimString (szValue, sizeof (szValue));

                TUtil_Uppcase (szName);

                mapWhoisFields [(const char*)szName] = (const char*) szValue;

                //TRACE ("(%u) Name [%s] - Value [%s]\n", mapWhoisFields.size(), szName, (*this) [szName]);
            }
        }

    } catch (Exception* pEx)
    {
        //TRACE ("Error: [%s]\n", pEx->GetExceptionMessage());
        objSocket.Close();
    }

    if (mapWhoisFields.size() == 0)
    {
        return false;
    }

    return true;
}





bool whois::Lookup (const char* pszValue, uint16_t nStrLen)
{

    const char* pszDtValue;

    //Processing Cache first of all
    if (nCacheTimeout > 0)
    {
        string strValue;
        map<string, struct WhoisCache>::iterator mapIterator;

        strValue.append (pszValue, nStrLen);

        NOTRACE ("Value: [%s]\n", strValue.c_str());

        mapIterator = mapWhoisCache.find (strValue.c_str());

        if (mapIterator != mapWhoisCache.end())
        {
            NOTRACE (" *** CACHE ***"); //sleep (1);
            mapWhoisFields = mapIterator->second.mapWhoisFields;

            return true;
        }
    }

    //ProcessWhois ("www.google.com", pszValue, objVariables);
    if (ProcessWhois (WHOIS_DEFAULT, pszValue) == false)
    {
        return false;
    }

    ;

    if ((pszDtValue = (*this) ["status"])== NULL && strcmp (pszDtValue, "ALLOCATED") != 0)
    {
        return false;
    }

    if ((pszDtValue = (*this) ["refer"])== NULL)
    {
        return false;
    }

    string strValue = pszDtValue;

    if (ProcessWhois (strValue.c_str(), pszValue) == false)
    {
        return false;
    }

    NOTRACE ("CACHE: [%u]\n", nCacheTimeout);

    if (nCacheTimeout > 0) ProcessCache (pszValue, nStrLen, mapWhoisFields);
    
    return true;
}








const char* whois::operator[] (const char* pszVariable)
{
    //Fist lets check if the Attribute is available

    string str = pszVariable;

    std::transform(str.begin(), str.end(),str.begin(), ::toupper);

    map<string,string>::iterator mapI;

    if ((mapI = mapWhoisFields.find (str.c_str())) == mapWhoisFields.end())
    {
        return NULL;
    }


    return mapI->second.c_str();
}




void whois::SetCacheTimeout (uint32_t nCacheTimeout)
{
    this->nCacheTimeout = nCacheTimeout;
}



void whois::ProcessCache (const char* pszValue, uint16_t nStrLen, map<string,string>& mapWhoisFields)
{
    map<string, struct WhoisCache>::iterator mapInterator;
    time_t timeNow;

    for (mapInterator = mapWhoisCache.begin(); mapInterator != mapWhoisCache.end(); mapInterator++)
    {
        //processing old entries
        timeNow = time (NULL);

        if ((timeNow  - mapInterator->second.tEntrytime) > nCacheTimeout)
        {
            //TRACE ("Cache Erasing: [%s]\n", mapInterator->first.c_str());

            mapWhoisCache.erase(mapInterator);

            continue;
        }
        else if (mapInterator->first.compare (1, nStrLen, pszValue) == 0)
        {
            return;
        }

        //TRACE ("List: [%s]\n", mapInterator->first.c_str());
    }

    //If the loop has retch here, means pszValue must be stored.


    struct WhoisCache stWhoisCache;

    //stWhoisCache.strValue.append(pszValue, nStrLen);
    stWhoisCache.mapWhoisFields = mapWhoisFields;
    stWhoisCache.tEntrytime = time (NULL);

    string strValue;

    strValue.append (pszValue, nStrLen);

    //TRACE ("Adding: [%s]\n", strValue.c_str());

    mapWhoisCache [strValue] = stWhoisCache;
}



uint32_t whois::GetCacheSize ()
{
    return (uint32_t) mapWhoisCache.size();
}
