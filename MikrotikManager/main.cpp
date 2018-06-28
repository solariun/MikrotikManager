//
//  main.cpp
//  MikrotikManager
//
//  Created by Gustavo Campos on 6/6/14.
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <unistd.h>
#include <Terminal.h>
#include <Variant.h>
#include "librouteros.h"
#include "main.h"
#include <algorithm>
#include <whois.h>

#include <stdio.h>

using namespace std;

map<string,string> mapHostDictionary;

map<string, struct IPUsage> pHostList;
string strIPInterfaceMonitoring;
bool bIPMonitoringConnected = false;
bool bIPMonitoringIP = false;
bool bProfileMonitor = false;


map<string, struct MktProfile *> mapProfiling;

vector<struct MktProfile*> vProfilling, vShowProfile;
uint32_t  nTotalProfiling;




map<string, struct Interfaces*> mapInterfaces;

extern char **environ;

uint nColumns;
uint nLines;

struct ros_connection *conn;
volatile int do_continue = 0;




map<string, struct IPMonitoring> mapMonitoring;
/* ------------------------------------- */



void handledata(struct ros_result *result) {
	int i;
    static string strData = "";

	if (result->re)
    {
		//printf("--\n");
        printf ("Line: %s\n", strData.c_str());
        strData.clear();
	}
	if (result->trap)
    {
		printf("!trap Error following: \n");
	}
	if (result->fatal)
    {
		printf("!fatal: \n");
	}

	for (i = 1; i < result->sentence->words; ++i)
    {
		//printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
	}
	if (result->done) {
		printf("== Ending command.\n\n");
        strData.clear();
	}
    
	ros_result_free(result);
}



struct IPUsage* ClainIPUsage (string strHost)
{
    map<string, struct IPUsage> pHost;
    map<string, struct IPUsage>::iterator pHosti;

    struct IPUsage* classIPusage;

    //Finding if the data exists
    pHosti = pHostList.find (strHost);

    
    if (pHosti == pHostList.end())
    {
        //TRACE ("Creating new memory for %s\n", strHost.c_str());
        classIPusage = new struct IPUsage;
        memset ((void*) classIPusage, 0, sizeof (struct IPUsage));
    }
    else
    {
        //TRACE ("Returning alread stored [%X] Key: [%s]\n", (*pHosti).second, (*pHosti).first.c_str());
        classIPusage = &(*pHosti).second;
    }

    return classIPusage;
}




void IPUsageHandle (struct ros_result *result)
{
	int i;
    static string strData = "";
    static char szNumber [30] = "";


	if (result->trap)
    {
		printf("!trap Error following: \n");
        bIPMonitoringIP = false;
	}
	if (result->fatal)
    {
		printf("!fatal: \n");
	}

	for (i = 1; i < result->sentence->words; ++i)
    {
		//printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
	}

    if (result->re)
    {
        static char szSrcAddress [40];
        static bool bFirst = true;

        szSrcAddress [0] = '\0';

        if (TUtil_GetValueFromResponse2 (strData.c_str(), "src-address", szSrcAddress, sizeof (szSrcAddress) - 1, ';', '=') == false)
        {
            snprintf (szSrcAddress, sizeof (szSrcAddress) - 1, "TOTAL");

            bFirst = false;
        }

        if (bFirst == true)
        {
            bFirst = false;

            /* Populating data */
            map<string, struct IPUsage>::iterator pHostListi;

            for (pHostListi = pHostList.begin(); pHostListi != pHostList.end(); pHostListi++)
            {
                pHostListi->second.nTXbps = 0;
                pHostListi->second.nTXPackets = 0;
                pHostListi->second.nRXbps = 0;
                pHostListi->second.nRXPackets = 0;
                pHostListi->second.nCountConnections = 0;
                pHostListi->second.pvecConnections.clear();
            }
        }



        struct IPUsage* pIPUsage = ClainIPUsage ((const char*) szSrcAddress);


        snprintf (pIPUsage->szSrcAddress, sizeof (pIPUsage->szSrcAddress) - 1, "%s", szSrcAddress);


        TUtil_GetValueFromResponse2 (strData.c_str(), "tx", szNumber, sizeof (szNumber) - 1, ';', '=');
        pIPUsage->nRXbps = strtod (szNumber, NULL) / 1024;

        TUtil_GetValueFromResponse2 (strData.c_str(), "tx-packets", szNumber, sizeof (szNumber) - 1, ';', '=');
        pIPUsage->nRXPackets = strtod (szNumber, NULL) / 1024;

        TUtil_GetValueFromResponse2 (strData.c_str(), "rx", szNumber, sizeof (szNumber) - 1, ';', '=');
        pIPUsage->nTXbps = strtod (szNumber, NULL) / 1024;

        TUtil_GetValueFromResponse2 (strData.c_str(), "rx-packets", szNumber, sizeof (szNumber) - 1, ';', '=');
        pIPUsage->nTXPackets = strtod (szNumber, NULL) / 1024;



        //printf("\n%s\n", strData.c_str());

        //TRACE ("Src-Address....: [%s]\n", pIPUsage->szSrcAddress);
        //TRACE ("TX: [%.2f] kbps - TX-packets: [%u]\n", (float) pIPUsage->nTXPackets / 8, pIPUsage->nTXPackets);
        //TRACE ("RX: [%.2f] kbps - RX-packets: [%u]\n", (float) pIPUsage->nRXPackets / 8, pIPUsage->nRXPackets);


        strData = pIPUsage->szSrcAddress ;

        //pIPUsage->pvecConnections = NULL;

        pHostList [strData] = *pIPUsage;

        //delete pIPUsage;

        strData.clear();
	}


    if (result->done)
    {
		printf("== Ending command.\n\n");
        strData.clear();
	}
    
	ros_result_free(result);
}



void IPNameStructure (struct ros_result *result)
{
	int i;
    static string strData = "";
    bool bFirst = true;



	if (result->trap)
    {
		printf("!trap Error following: \n");
	}
	if (result->fatal)
    {
		printf("!fatal: \n");
	}

	for (i = 1; i < result->sentence->words; ++i)
    {
		//printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
	}

    if (result->re)
    {
        static char szSrcAddress [40];
        static string strAddress;

        if (bFirst == true)
        {
            //pHostList.clear();
            bFirst = false;
        }

        TUtil_GetValueFromResponse2 (strData.c_str(), "address", szSrcAddress, sizeof (szSrcAddress) - 1, ';', '=');

        strAddress = szSrcAddress;


        struct IPUsage* pIPUsage = ClainIPUsage ((const char*) szSrcAddress);

        snprintf (pIPUsage->szSrcAddress, sizeof (pIPUsage->szSrcAddress) - 1, "%s", szSrcAddress);
        //TUtil_GetValueFromResponse2 (strData.c_str(), "address", pIPUsage->szSrcAddress, sizeof (pIPUsage->szSrcAddress) - 1, ';', '=');

        TUtil_GetValueFromResponse2 (strData.c_str(), "mac-address", pIPUsage->szMacAddress, sizeof (pIPUsage->szMacAddress) - 1, ';', '=');

        TUtil_GetValueFromResponse2 (strData.c_str(), "host-name", pIPUsage->szHostName, sizeof (pIPUsage->szHostName) - 1, ';', '=');

/*
		printf("\n%s\n", strData.c_str());
        
        TRACE  ("Src-Address....: [%s]\n", pIPUsage->szSrcAddress);
        TRACE  ("Mac-Address....: [%s]\n", pIPUsage->szMacAddress);
        TRACE  ("Host.Name......: [%s]\n", pIPUsage->szHostName);
*/

        strData = pIPUsage->szSrcAddress ;

        pHostList [strData] = *pIPUsage;


        strData.clear();
	}


    if (result->done)
    {
        strData.clear();
        bFirst = true;
	}

	ros_result_free(result);
}


bool ShowIPMonitoringSortby (const struct IPConnectionMonitor& pA, const struct IPConnectionMonitor& pB)
{
    return (pA.nTX + pA.nTX > pB.nTX + pB.nRX);
}



bool ShowUsageBoxesSortby (const struct IPUsage& pA, const struct IPUsage& pB)
{
    if (pA.szSrcAddress [0] == 'T') return true;

    if ((pA.nTXbps * pA.nCountConnections) > (pB.nTXbps * pB.nCountConnections))
    {
        return true;
    }

    return false;
}



void ShowUsageBoxes (map<string, struct IPUsage>& pHostList)
{
    vector <struct IPUsage>  pVector;
    vector <struct IPUsage>::iterator  pVectori;

    /* Populating data */
    map<string, struct IPUsage>::iterator pHostListi;

    for (pHostListi = pHostList.begin(); pHostListi != pHostList.end(); pHostListi++)
    {
        pVector.push_back ((*pHostListi).second);
    }

    /* Sorting data */
    sort (pVector.begin(), pVector.end(), ShowUsageBoxesSortby);

    uint nColumns;
    uint nLines;
    uint32_t nCount;
    Term.GetTermSize (&nColumns, &nLines);

    nColumns -= 63;
    nLines   -= (mapInterfaces.size() + 2);



    //TRACE ("Collumns: [%u]  Lines: [%u]\n", nColumns, nLines);

    Term.ResetAttr();
    Term.Box (3, 2, nLines, nColumns, "Live trafic monitoring");

    uint nX, nY;

    nX  = 5; nY = 4;

#define BOXX 7
#define BOXY 20

    struct IPUsage* pHost;
    uint  nBGColor;

    for (pVectori = pVector.begin(); pVectori != pVector.end(); pVectori++)
    {
        //printf ("%10X: Host: [%-20s] MAC: [%s] - [%s] TX: [%-.2f] RX: [%-.2f] \n", &(*pVectori), (*pVectori).szSrcAddress, (*pVectori).szMacAddress, (*pVectori).szHostName, (float) (*pVectori).nRXbps / 8, (float) (*pVectori).nTXbps / 8);

        pHost = (struct IPUsage*) (&(*pVectori));

        if (pHost->nRXbps == 0 && pHost->nTXbps == 0 && pHost->nCountConnections == 0)
        {
            continue;
        }


        if (pHost->nTXbps > ServerAttr.nTXLimit || pHost->nCountConnections > 20)
        {
            nBGColor = 1;
        }
        else if ((float)  (pHost->nTXbps) > ((uint) ServerAttr.nTXLimit * 0.70))
        {
            nBGColor = 3;
        }
        else if (pHost->nRXbps > ServerAttr.nRXLimit)
        {
            nBGColor = 6;
        }
        else
        {
            nBGColor = 2;
        }


        Term.Color (0, 7, nBGColor);

        Term.ClearBox (nX, nY, nX + BOXX, nY + BOXY);

        Term.ColorBright();
        Term.Locate (nX + 1, nY + 2);
        printf ("%s", pHost->szSrcAddress);


        Term.Color (0, 3, nBGColor);
        Term.Locate (nX + 2, nY + 2);
        Term.ColorBright ();
        printf ("%.15s\n", mapHostDictionary.find (pHost->szHostName) == mapHostDictionary.end () ? pHost->szHostName : mapHostDictionary [pHost->szHostName].c_str());

        Term.Color (0, 7, nBGColor);

        Term.Locate (nX + 3, nY + 2);

        if (pHost->nCountConnections > 10) Term.ColorBlink();

        Term.ColorBright ();
        printf ("SOK: %u",  pHost->nCountConnections);


        Term.Color (0, 7, nBGColor);

        Term.Locate (nX + 4, nY + 3); Term.ColorBright ();
        printf ("TX: %u kbps",  pHost->nTXbps);

        Term.Locate (nX + 5, nY + 3); Term.ColorBright ();
        printf ("RX: %u kbps", pHost->nRXbps);

        Term.Color (0, 4, nBGColor); Term.Locate (nX + 7, nY + 2);
        printf ("%s", pHost->szMacAddress);


        nY += (BOXY + 2);

        if (nY + (BOXY + 2) > nColumns) { nY = 4; nX += (BOXX + 2); }

        if (nX + (BOXX + 2) > nLines) break;
    }





    {
        uint nX = 5;

        whois objWhois;
        objWhois.SetCacheTimeout (86400);

        Term.Locate (nX-2, nColumns + 1);
        Term.ColorInvert(); printf ("%-31s", "IP Address     TX Kbps  RX Kbps");

        Term.Color (0,2,0); Term.ColorNormal();

        bool bInfo = false;

        for (pVectori = pVector.begin(); pVectori != pVector.end() && nX < nLines + 2; pVectori++)
        {
            if (pVectori->pvecConnections.size() > 0)
            {
                Term.Locate (nX++, nColumns + 1);

                if (mapHostDictionary.find (pVectori->szHostName) == mapHostDictionary.end())
                {
                    Term.ColorInvert(); printf ("IP: %-27s", pVectori->szSrcAddress);
                }
                else
                {
                    Term.ColorInvert(); printf ("IP: %-27s", mapHostDictionary [(const char*) pVectori->szHostName].c_str());
                }

                if (pVectori->pvecConnections.size() > 1)
                {
                    sort (pVectori->pvecConnections.begin(), pVectori->pvecConnections.end(), ShowIPMonitoringSortby);
                }

                nCount = 0;
                for (vector <struct IPConnectionMonitor>::iterator iteMon = pVectori->pvecConnections.begin(); iteMon != pVectori->pvecConnections.end() && nX < nLines + 2; iteMon++)
                {
                    {
                        Term.Locate (nX++, nColumns + 1);
                        Term.ColorNormal ();
                        if ((iteMon->nRX / 1024) > ServerAttr.nRXLimit / 0.5)
                        {
                            Term.Color (0, 7, 1);
                            Term.ColorBright();
                            Term.ColorBlink();

                            bInfo = true;
                        }
                        else if  ((iteMon->nTX / 1024) > ServerAttr.nRXLimit / 0.5)
                        {
                            Term.Color (0, 7, 6);
                            Term.ColorBright();
                            Term.ColorBlink();

                            bInfo = true;
                        }

                        printf ("%-15s %7.1f %7.1f", iteMon->szDstAddress.c_str(),(double)iteMon->nRX / 1024 , (double)iteMon->nTX /1024);

                        if (bInfo == true)
                        {
                            Term.Color (0, 7, 4);

                            string strCommand = "";
                            bInfo = false;

                            fflush (stdout);
                            try
                            {
                                if ((objWhois.Lookup (iteMon->szDstAddress.c_str(), iteMon->szDstAddress.length())) ==true)
                                {
                                    if (objWhois ["OrgName"] != NULL)
                                    {
                                        Term.Locate (nX, nColumns + 1); printf ("%-31s", " "); Term.Locate (nX++, nColumns + 1);printf ("%.31s", objWhois ["OrgName"]);
                                    }

                                    if (objWhois ["owner"] != NULL)
                                    {
                                        Term.Locate (nX, nColumns + 1); printf ("%-31s", " ");Term.Locate (nX++, nColumns + 1); printf ("%.31s", objWhois ["owner"]);
                                    }

                                    if (objWhois ["person"] != NULL)
                                    {
                                        Term.Locate (nX, nColumns + 1); printf ("%-31s", " ");Term.Locate (nX++, nColumns + 1); printf ("%.31s", objWhois ["person"]);
                                    }

                                    if (objWhois ["country"] != NULL)
                                    {
                                        Term.Locate (nX, nColumns + 1); printf ("%-31s", " ");Term.Locate (nX++, nColumns + 1); printf ("%.31s", objWhois ["country"]);
                                    }
                                }
                            }
                            catch (Exception *pEx)
                            {
                                //Just catch and follow from there, no need to do anything, it will be carried out automatically.
                            }

                        }

                        Term.ColorNormal();
                    }
                }
            }
        }

        /*
         for (map<string, struct IPUsage*>::iterator iteIP = pHostList.begin(); iteIP != pHostList.end() && nX < nLines; iteIP++)
         {
         if (iteIP->second->pvecConnections != NULL && iteIP->second->pvecConnections->size() > 0)
         {
         Term.Locate (nX++, nColumns + 1);
         Term.ColorInvert(); printf ("%-29s", iteIP->second->szSrcAddress);


         for (vector <struct IPConnectionMonitor>::iterator iteMon = iteIP->second->pvecConnections->begin(); iteMon != iteIP->second->pvecConnections->end() && nX < nLines; iteMon++)
         {
         if (&(*iteMon) != NULL)
         {
         Term.Locate (nX++, nColumns + 1);
         Term.ColorInvert(); printf ("  %-29s %8u %8u", "", iteMon->nTX, iteMon->nRX);
         }
         }
         
         }
         }
         */
    }

    fflush (stdout);

    Term.ResetAttr();

    Term.Locate (1, 1);

    fflush (stdout);
}



/* Sorting procedures for Router Usage Profile */
bool ProfileSort (struct MktProfile* pA, struct MktProfile* pB)
{
    return pA->nUsage > pB->nUsage;
}


/* BEST WAY TO PROCESS A ONLINE REQUEST SO FAR */
void ProfileHandler (struct ros_result *result)
{
    int i;
    static string strData = "";
    static Variant objVar;
    static bool bFirst = true;

    bool bOK = false;

    if (result->trap)
    {
        printf("!trap Error following: \n");
        bProfileMonitor = false;
    }
    
    if (result->fatal)
    {
        printf("!fatal: \n");
    }
    else
    {
        bOK = true;
    }

    //printf ("bOK: [%s] - Fatal: [%s] Words: [%d]\n", bOK ? "true" : "false", result->fatal ? "true" : "false", result->sentence->words);
    
    for (i =1; i < result->sentence->words; ++i)
    {
        //printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
    }

    
    if (bOK == true)
    {
        objVar = strData;

        //TRACE ("data: [%s] objVar=[%d]\n", strData.c_str(), objVar.GetCounter());
        
        try
        {

            if (bFirst == true)
            {
                map<string, struct MktProfile*>::iterator mapProfilei;
                //Zeroing data for next collecting event.
                for (mapProfilei = mapProfiling.begin(); mapProfilei != mapProfiling.end(); mapProfilei ++)
                {
                    mapProfilei->second->nUsage = 0;
                }
                
                bFirst = false;
            }

            struct MktProfile* pProfile;


            string strType = objVar ["name"];

            if (mapProfiling.find(strType.c_str()) == mapProfiling.end())
            {
                pProfile = new struct MktProfile;
                pProfile->nUsage = 0;
            }
            else
            {
                pProfile = mapProfiling.find (strType)->second;
            }


            pProfile->strType = objVar ["name"];
            pProfile->nUsage += objVar.GetInteger ("usage");

            //TRACE ("Counter: [%u] [%s]\n", objVar.GetCounter(), objVar["name"]);

            strData = "";

            mapProfiling [strType] = pProfile;

            
            if (strcmp(objVar["name"], "total") == 0)
            {
                /* Sorting procedures */
                vShowProfile.clear();

                map<string, struct MktProfile*>::iterator mapProfilei;

                nTotalProfiling = 1;
                
                for (mapProfilei = mapProfiling.begin(); mapProfilei != mapProfiling.end(); mapProfilei ++)
                {
                    vShowProfile.push_back  (mapProfilei->second);
                    nTotalProfiling += mapProfilei->second->nUsage;
                }


                /* Sorting data */
                sort (vShowProfile.begin(), vShowProfile.end(), ProfileSort);


                vector<struct MktProfile*>::iterator vProfilei;

                for (vProfilei = vShowProfile.begin(); vProfilei != vShowProfile.end(); vProfilei++)
                {
                    //TRACE ("Total: [%d] Name: %-40s %u\n",nTotalProfiling,  (*vProfilei)->strType.c_str(), (*vProfilei)->nUsage);
                }
                
            }

        }
        catch (string& pEx)
        {
            YYTRACE ("Error: [%s]\n", pEx.c_str());
        }
    }


    if (result->re)
    {

    }

    if (result->done)
    {
        strData = "";
        bFirst = true;
    }

    ros_result_free(result);

}




void HealthHandler (struct ros_result *result)
{
    int nCount;
    static string strData = "";
    static Variant objVar;
    static bool bFirst = true;
    bool bOK = false;

    if (ServerAttr.bHealthSupport == true)
    {

        if (result->trap)
        {
            printf("!trap Error following: \n");
        }
        if (result->fatal)
        {
            printf("!fatal: \n");
        }
        else
        {
            bOK = true;
        }

        for (nCount = 1; nCount < result->sentence->words; ++nCount)
        {
            if (result->trap || result->fatal)
            {
                printf ("%s\n", result->sentence->word [nCount]);
            }
            else
            {
                strData = strData + ((char *) &result->sentence->word [nCount][1]) + ";";
            }
        }


        if (bOK == true)
        {
            objVar = strData;


            try
            {
                if (objVar.GetCounter() > 1)
                {

                    if (bFirst == true)
                    {
                        bFirst = false;
                    }


                    ServerAttr.nCPUVoltage = objVar.GetInteger ("voltage");
                    ServerAttr.nAmbienteTemp = objVar.GetInteger ("temperature");
                    ServerAttr.nCPUTemp = objVar.GetInteger ("cpu-temperature");

                    ServerAttr.bHealthSupport = true;
                }
                else //Ending
                {
                    //TRACE ("End procedures");
                }

            }
            catch (string& pEx)
            {
                ServerAttr.bHealthSupport = false;
                return;
            }
        }


        if (result->re)
        {
            
        }
        
        if (result->done)
        {
            strData = "";
            bFirst = true;
        }
    }

    ros_result_free(result);
}




void DefaultHandler (struct ros_result *result)
{
    int i;
    static string strData = "";
    static Variant objVar;

    if (result->trap)
    {
        printf("!trap Error following: \n");
    }
    if (result->fatal)
    {
        printf("!fatal: \n");
    }

    for (i =1; i < result->sentence->words; ++i)
    {
        //printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
    }

    objVar = strData;

    if (result->re)
    {
        //none to do
        //strData.clear();
        TRACE ("RE - OBJ: [%u] - Data: [%s]\n\n", objVar.GetCounter(),  strData.c_str() );

        strData.clear();
    }
    else if (result->done)
    {
        //TRACE ("DATA: [%s]\n", strData.c_str());

        TRACE ("DONE\n");
        strData.clear ();
    }
    else
    {
        TRACE ("OBJVAR: [%u]\n\n", objVar.GetCounter());
    }

    ros_result_free(result);

}



void ResourceHandler (struct ros_result *result)
{
    int i;
    static string strData = "";
    static Variant objVar;

    if (result->trap)
    {
        printf("!trap Error following: \n");
    }
    if (result->fatal)
    {
        printf("!fatal: \n");
    }

    for (i =1; i < result->sentence->words; ++i)
    {
        //printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
    }

    if (result->re)
    {
        //none to do
        //strData.clear();
    }

    if (result->done)
    {
        //TRACE ("DATA: [%s]\n", strData.c_str());
        
        objVar = strData;
        try
        {
            ServerAttr.nRouterCPULoad = objVar.GetInteger ("cpu-load");
            //TRACE ("CPU: [%s]\n", objVar ["cpu-load"]);

            ServerAttr.strUpTime = objVar ["uptime"];
            ServerAttr.strRouterVersion = objVar ["version"];
            ServerAttr.nFreeMemory = objVar.GetInteger  ("free-memory");
            ServerAttr.nTotalMemory = objVar.GetInteger ("total-memory");
            ServerAttr.strCPU = objVar ["cpu"];
            ServerAttr.nCPUCounting = objVar.GetInteger ("cpu-count");
            ServerAttr.nCPUFrequence = objVar.GetInteger("cpu-frequency");
            ServerAttr.strArchitecture = objVar ["architecture-name"];
            ServerAttr.strBoardName = objVar ["board-name"];
            ServerAttr.strPlatform  = objVar ["platform"];

            ServerAttr.nFreeHDDSpace = objVar.GetInteger  ("free-hdd-space");
            ServerAttr.nTotalHDDSpace = objVar.GetInteger ("total-hdd-space");

        }
        catch (const string& pEx)
        {
            YYTRACE ("Error, recovering cpu-load. Error: [%s]\n", pEx.c_str ());
        }
        
        strData.clear ();
    }
    
    ros_result_free(result);
}





void Render ()
{
    uint32_t nCount;

    static Terminal::TGraph objCPUGraph, objMemGraph;

    Term.ResetAttr();

    //Term.Cls ();

    Term.GetTermSize (&nColumns, &nLines);


    Term.Color (0, 4, 7); Term.ColorBright();
    
    Term.Locate (3, nColumns - 30);
    printf ("%-30s", "CPU Load (Porcentage)");
    
    Term.ResetAttr();
    Term.Color (0, 0, 7);
    Term.Box (4, nColumns-30, 10, nColumns, NULL);

    objCPUGraph = (float) ServerAttr.nRouterCPULoad;

    objCPUGraph.PlotGraphics (11, nColumns - 30, 30, 7, 100);
    
    if (ServerAttr.nRouterCPULoad > 70)
    {
        Term.Color (5, 1, 7);
    }
    else if (ServerAttr.nRouterCPULoad > 50)
    {
        Term.Color (0, 3, 7); Term.ColorBright();
    }
    else
    {
        Term.Color (0, 4, 7); Term.ColorBright();
    }
    
    Term.PrintBigNum (5, nColumns-25, (float) ServerAttr.nRouterCPULoad, true);

    Term.Color ( 0, 0, 7);

    Term.Box (19, nColumns - 30, 36, nColumns, NULL);

    Term.Locate(20, nColumns - 28); printf ("CPU    : [%-.16s]", ServerAttr.strCPU.c_str());
    Term.Locate(21, nColumns - 28); printf ("Core   : [%u - %uMhz]", ServerAttr.nCPUCounting, ServerAttr.nCPUFrequence);
    Term.Locate(22, nColumns - 28); printf ("Archit.: [%-.16s]", ServerAttr.strArchitecture.c_str());
    Term.Locate(23, nColumns - 28); printf ("Router : [%-.16s]", ServerAttr.strBoardName.c_str());
    Term.Locate(24, nColumns - 28); printf ("Version: [%-.16s]", ServerAttr.strRouterVersion.c_str());

    Term.Locate(26, nColumns - 28); printf ("Memory : [%.1f M]", (float) ServerAttr.nTotalMemory / 1048576);
    Term.Locate(27, nColumns - 28); printf ("Free   : [%.1f M]", (float) ServerAttr.nFreeMemory / 1048576);


    Term.Locate (35, nColumns - 28);
    if (ServerAttr.bHealthSupport == true)
        printf ("V: %uV Amb: %uC CPU: %uC", ServerAttr.nCPUVoltage, ServerAttr.nAmbienteTemp, ServerAttr.nCPUTemp);
    else
        printf ("  Health not supported.");

    objMemGraph = (float) (ServerAttr.nTotalMemory - ServerAttr.nFreeMemory) ;
    objMemGraph.PlotGraphics (28, nColumns-28, 26, 6, (float) ServerAttr.nTotalMemory);


    Term.Color (1, 4 , 4);

    Term.Box (37, nColumns - 30,(int) vShowProfile.size() + 37 + 1, nColumns, "Processing usage");

    vector<struct MktProfile*>::iterator vProfilei;


    {
        nCount = 0;
        uint32_t nPorcentage;

        Term.Locate ((int)vShowProfile.size() + 37 + 2, nColumns-30); printf ("Cycle:%-24u", nTotalProfiling);

        if (nTotalProfiling > 0) for (vProfilei = vShowProfile.begin(); vProfilei != vShowProfile.end(); vProfilei++)
        {
            Term.Locate (38 + nCount++, nColumns - 28);

            nPorcentage =  ((*vProfilei)->nUsage * 100) / nTotalProfiling;

            if (nPorcentage > 70)
            {
                Term.Color (5, 1, 4);
            }
            else if (nPorcentage > 50)
            {
                Term.Color (1, 3, 4);
            }
            else
            {
                Term.Color (0, 7, 4);
            }

            printf ("%-20s %u%%\n", (*vProfilei)->strType.c_str(), nPorcentage);

        }

        Term.ResetAttr();
    }


    Term.Locate (1, 1);
    Term.BigChar();
    Term.Color (0, 3, 4);

    printf ("%*s\rMikrotik Monitorig", (uint)(nColumns / 2), " ");
    Term.ResetAttr();
    Term.Locate (2, 1);
    printf ("Roteador: [%s] Interface: [%s] Thresholds TX: [%u] RX: [%u] WhoisCache: [%u]", ServerAttr.pszRouterAddress, ServerAttr.pszInterface, ServerAttr.nTXLimit, ServerAttr.nRXLimit, whois::GetCacheSize ());
    
    ShowUsageBoxes (pHostList);

    map<string, struct Interfaces*>::iterator mapInterfacei;
    nCount = 0;

    if (mapInterfaces.size() > 0)
    {
        Term.Color (0, 0, 7);
        Term.Locate ((int) (nCount++ + nLines - mapInterfaces.size() - 1), 1); printf ("%*s\n", (nColumns - 32) * -1, "ACT| Interface Name       | Type     | TX           | RX           | TX Errors |RX Errors |TX Drops  | RX Drops");


        fflush (stdout);
        printf ("\n");
        Term.ColorUnderScore();

        for (mapInterfacei = mapInterfaces.begin(); mapInterfacei != mapInterfaces.end(); mapInterfacei++)
        {
            if (mapInterfacei->second->strDisable [0] == 't')
            {
                Term.Color (0, 4, 3);
            }
            else if ((nCount % 2) == 0)
            {
                Term.Color (0, 7, 4);
            }
            else
            {
                Term.Color (0, 7, 0);
            }


            Term.Locate ((int)(nCount++ + nLines - mapInterfaces.size() - 1), 1); printf ("%*s\r %c | %-20s | %-8s | %8.1f Kbps| %8.1f Kbps| %8u  | %8u | %8u | %8u", nColumns - 32, "",
                                                             mapInterfacei->second->strDisable [0] == 'f' ? 'R' : 'X',
                                                             mapInterfacei->second->strName.c_str(),
                                                             mapInterfacei->second->strType.c_str(),
                                                             (float) mapInterfacei->second->nTX_bits_seconds / 1024,
                                                             (float) mapInterfacei->second->nRX_bits_seconds / 1024,
                                                             mapInterfacei->second->nTX_error_seconds / 1024,
                                                             mapInterfacei->second->nRX_error_seconds / 1024,
                                                             mapInterfacei->second->nTX_drops_seconds / 1024,
                                                             mapInterfacei->second->nRX_drops_seconds / 1024);

        }
    }
}



void InterfacesDiscoverHandler (struct ros_result *result)
{
    uint nCount;
    static string strData = "";
    static Variant objVar;
    static bool bFirst = true;
    bool bOK = false;

    if (result->trap)
    {
        printf("!trap Error following: \n");
    }
    if (result->fatal)
    {
        printf("!fatal: \n");
    }
    else
    {
        bOK = true;
    }

    for (nCount = 1; nCount < result->sentence->words; ++nCount)
    {
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [nCount]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [nCount][1]) + ";";
        }
    }

    if (bOK == true)
    {
        objVar = strData;


        try
        {
            if (objVar.GetCounter() > 1)
            {

                if (bFirst == true)
                {
                    bFirst = false;
                }

                //TRACE ("STRDATA: [%s]\n\n", strData.c_str());

                struct Interfaces* pstInterface;

                if (mapInterfaces.find (objVar ["name"]) == mapInterfaces.end ())
                {
                    pstInterface = new struct Interfaces;

                    pstInterface->nTX_bits_seconds = 0;
                    pstInterface->nRX_bits_seconds = 0;

                    pstInterface->nTX_error_seconds = 0;
                    pstInterface->nRX_error_seconds = 0;

                    pstInterface->nTX_drops_seconds = 0;
                    pstInterface->nRX_drops_seconds = 0;

                    pstInterface->strName = objVar ["name"];
                }
                else
                {
                    pstInterface = mapInterfaces.find (objVar ["name"])->second;
                }

                pstInterface->strType = objVar ["type"];
                pstInterface->strRunning = objVar ["running"];
                pstInterface->strDisable = objVar ["disabled"];

                //TRACE ("Processing: [%s]\n", pstInterface->strName.c_str());

                mapInterfaces [pstInterface->strName] = pstInterface;
            }
            else //Ending
            {

            }

            strData = "";

        }
        catch (string& pEx)
        {
            YYTRACE ("Error: [%s]\n", pEx.c_str());

            strData = "";
        }
    }


    if (result->re)
    {
        
    }
    
    if (result->done)
    {
        strData = "";
        bFirst = true;
    }

    ros_result_free(result);
}



void NullHandler (struct ros_result *result)
{
    return;
}



void InterfaceDataHandler (struct ros_result *result)
{
    uint nCount;
    static string strData = "";
    static Variant objVar;
    static bool bFirst = true;
    bool bOK = false;

    if (result->trap)
    {
        //printf("!trap Error following: \n");
    }
    if (result->fatal)
    {
        printf("!fatal: \n");
    }
    else
    {
        bOK = true;
    }

    for (nCount = 1; nCount < result->sentence->words; ++nCount)
    {
        if (result->trap || result->fatal)
        {
            //printf ("%s\n", result->sentence->word [nCount]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [nCount][1]) + ";";
        }
    }

    if (bOK == true)
    {
        objVar = strData;


        try
        {
            if (objVar.GetCounter() > 1)
            {

                if (bFirst == true)
                {

                    bFirst = false;
                }

                struct Interfaces* pstInterface;

                if (mapInterfaces.find (objVar ["name"]) == mapInterfaces.end ())
                {
                    //Discarting till it existis on the map object
                    strData = "";
                    return;
                }
                else
                {
                    pstInterface = mapInterfaces.find (objVar ["name"])->second;
                }

                pstInterface->nTX_bits_seconds = objVar.GetInteger ("tx-bits-per-second");
                pstInterface->nTX_drops_seconds = objVar.GetInteger ("tx-drops-per-second");
                pstInterface->nTX_error_seconds = objVar.GetInteger ("tx-errors-per-second");

                pstInterface->nRX_bits_seconds = objVar.GetInteger ("rx-bits-per-second");
                pstInterface->nRX_drops_seconds = objVar.GetInteger ("rx-drops-per-second");
                pstInterface->nRX_error_seconds = objVar.GetInteger ("rx-errors-per-second");

                //TRACE ("STRDATA: [%s]\n", strData.c_str());
            }
            else //Ending
            {

            }

            strData = "";
        }
        catch (string& pEx)
        {
            YYTRACE ("Error: [%s]\n", pEx.c_str());
        }
    }


    if (result->re)
    {
        
    }
    
    if (result->done)
    {
        strData = "";
        bFirst = true;
    }

    ros_result_free(result);
}




void IPMonitoringHandler (struct ros_result *result)
{
    int i;
    static string strData = "";
    static Variant objVar;
    static uint32_t nTotal = 0;

    if (result->trap)
    {
        printf("!trap Error following: \n");
        bIPMonitoringConnected = false;
        strData.clear();
    }
    if (result->fatal)
    {
        printf("!fatal: \n");
    }

    for (i =1; i < result->sentence->words; ++i)
    {
        //printf(">%s\n", result->sentence->word[i]);
        if (result->trap || result->fatal)
        {
            printf ("%s\n", result->sentence->word [i]);
        }
        else
        {
            strData = strData + ((char *) &result->sentence->word [i][1]) + ";";
        }
    }

    objVar = strData;

    if (result->re)
    {
        //none to do
        //strData.clear();
        //TRACE ("RE - OBJ: [%u] - Data: [%s]\n\n", objVar.GetCounter(),  strData.c_str() );

        try
        {
            if (objVar.IsAvailable ("src-address") == true)
            {
                struct IPMonitoring stMonitor;
                struct IPMonitoring* pMonitor;

                stMonitor.szSrcAddress = objVar ["src-address"];

                if (mapMonitoring.find (stMonitor.szSrcAddress.c_str()) == mapMonitoring.end())
                {
                    stMonitor.nRX = 0;
                    stMonitor.nRXPacket = 0;

                    stMonitor.nTX = 0;
                    stMonitor.nTXPacket = 0;

                    mapMonitoring [stMonitor.szSrcAddress.c_str()] = stMonitor;
                }

                pMonitor =  &(mapMonitoring.find (stMonitor.szSrcAddress.c_str())->second);

                struct IPConnectionMonitor stIPConn;

                stIPConn.szDstAddress = objVar ["dst-address"];

                stIPConn.nTX = objVar.GetInteger ("tx");
                pMonitor->nTX = pMonitor->nTX + stIPConn.nTX;

                stIPConn.nTXPacket = objVar.GetInteger ("tx-packets");
                pMonitor->nTXPacket += stIPConn.nTXPacket;



                stIPConn.nRX = objVar.GetInteger ("rx");
                pMonitor->nRX = pMonitor->nRX + stIPConn.nRX;

                stIPConn.nRXPacket = objVar.GetInteger ("rx-packets");
                pMonitor->nRXPacket += stIPConn.nRXPacket;

                nTotal++;

                pMonitor->vecConnections.push_back (stIPConn);

                //printf  ("ADD data: [%s]\n", strData.c_str());
                //printf  ("  -> TX: [%u] kbps RX: [%u] kbps\n\n", stIPConn.nTX, stIPConn.nRX );
            }
            else if (objVar.IsAvailable ("tx") == true)
            {
                /* First lets show for developing procedures */

                struct IPMonitoring* pIPMonitor;
                //struct IPConnectionMonitor* pIPConn;

                struct IPUsage* pIPUsage = NULL;


                {
                    struct IPMonitoring stMonitor;
                    struct IPMonitoring* pMonitor;

                    stMonitor.szSrcAddress = "TOTAL";

                    if (mapMonitoring.find (stMonitor.szSrcAddress.c_str()) == mapMonitoring.end())
                    {
                        stMonitor.nRX = 0;
                        stMonitor.nRXPacket = 0;

                        stMonitor.nTX = 0;
                        stMonitor.nTXPacket = 0;

                        mapMonitoring [stMonitor.szSrcAddress.c_str()] = stMonitor;
                    }

                    pMonitor =  &(mapMonitoring.find (stMonitor.szSrcAddress.c_str())->second);

                    struct IPConnectionMonitor stIPConn;

                    stIPConn.nTX = objVar.GetInteger ("tx");
                    pMonitor->nTX = pMonitor->nTX + stIPConn.nTX;

                    stIPConn.nTXPacket = objVar.GetInteger ("tx-packets");
                    pMonitor->nTXPacket += stIPConn.nTXPacket;


                    stIPConn.nRX = objVar.GetInteger ("rx");
                    pMonitor->nRX = pMonitor->nRX + stIPConn.nRX;

                    stIPConn.nRXPacket = objVar.GetInteger ("rx-packets");
                    pMonitor->nRXPacket += stIPConn.nRXPacket;
                    
                    pMonitor->vecConnections.push_back (stIPConn);

                }

                nTotal = 0;

                /*
                for (map<string, struct IPUsage*>::iterator iteItem = pHostList.begin(); iteItem != pHostList.end(); iteItem++)
                {
                    iteItem->second->nCountConnections = 0;
                    //iteItem->second->nTXbps = 0;
                    //iteItem->second->nRXbps = 0;
                    //iteItem->second->nTXPackets = 0;
                    //iteItem->second->nRXPackets = 0;
                }
                 */

                for (map<string, struct IPMonitoring>::iterator iteIP = mapMonitoring.begin(); iteIP != mapMonitoring.end(); iteIP++)
                {
                    nTotal = nTotal + (uint32_t) iteIP->second.vecConnections.size ();
                }


                for (map<string, struct IPMonitoring>::iterator iteIP = mapMonitoring.begin(); iteIP != mapMonitoring.end(); iteIP++)
                {
                    pIPMonitor = &(*iteIP).second;

                    if (pIPMonitor->vecConnections.size() > 0)
                    {

                        /*
                        printf  ("IP [%s] TX: [%u - %u Kbps] RX: [%u - %u Kbps] Connections: TotalL [%u] \n", pIPMonitor->szSrcAddress.c_str(), (uint32_t)pIPMonitor->nTX , (uint32_t) ((float) pIPMonitor->nTX /1024) , (uint32_t) pIPMonitor->nRX , (uint32_t) ((float) pIPMonitor->nRX / 1024) , pIPMonitor->vecConnections.size());

                        for (vector <struct IPConnectionMonitor>::iterator iteItem = pIPMonitor->vecConnections.begin(); iteItem != pIPMonitor->vecConnections.end(); iteItem++)
                        {
                            pIPConn = & (*iteItem);

                            printf ("\t IP:[%-15s] TX %10u bps RX %10u bps Pkts: TX %10u RX %10u \n", pIPConn->szDstAddress.c_str(), pIPConn->nTX, pIPConn->nRX, pIPConn->nTXPacket, pIPConn->nRXPacket);
                        }
                        */

                        // =======================================================
                        // Processing current data to populate HostList
                        // adding or updating the total traffic data by IP
                        // =======================================================
                        if (pHostList.find (pIPMonitor->szSrcAddress.c_str()) == pHostList.end())
                        {
                            struct IPUsage pIPUsage;

                            // Make sure everything will be initilizaed as 0 or null
                            memset ((void*) &pIPUsage, 0, sizeof (pIPUsage));

                            //Naming the source of the request
                            snprintf (pIPUsage.szSrcAddress, sizeof (pIPUsage.szSrcAddress), "%s", pIPMonitor->szSrcAddress.c_str());

                            snprintf (pIPUsage.szHostName, sizeof (pIPUsage.szHostName), "Unkown");

                            snprintf (pIPUsage.szMacAddress, sizeof (pIPUsage.szMacAddress), "-- NOT MANAGED --");

                            //Adding to the back of the vector array object
                            pHostList [pIPMonitor->szSrcAddress.c_str()] = pIPUsage;
                        }


                        //Independent if added will reclain the pointer again.
                        pIPUsage = &(pHostList.find (pIPMonitor->szSrcAddress.c_str())->second);

                        //Starting populating data to the ongoing data.

                        if (pIPUsage->szSrcAddress [0] == 'T')
                        {
                            pIPUsage->nCountConnections = nTotal;
                            //pIPUsage->pvecConnections;
                            nTotal = 0;
                        }
                        else
                        {
                            pIPUsage->nCountConnections = (uint32_t) pIPMonitor->vecConnections.size();
                            pIPUsage->pvecConnections = pIPMonitor->vecConnections;
                        }



                        /*
                        //Addind data by nativily converting to kbps instead bps collected from the stream.
                        //This approuch will make a more accurate data

                        pIPUsage->nRXbps = (uint32_t) ((float) pIPMonitor->nTX /1024);
                        pIPUsage->nRXPackets = pIPMonitor->nTXPacket;

                        pIPUsage->nTXbps = (uint32_t) ((float) pIPMonitor->nRX / 1024);
                        pIPUsage->nTXPackets = pIPMonitor->nRXPacket;
                        */
                    }
                }


                mapMonitoring.clear();

            }


        }
        catch (string& pEx)
        {
            YYTRACE ("Error processing data: [%s] - DATA: [%s]\n", pEx.c_str(), strData.c_str());
        }



        strData.clear();
    }
    else if (result->done)
    {
        //TRACE ("DATA: [%s]\n", strData.c_str());

        TRACE ("DONE\n");
        strData.clear ();
    }
    else
    {
        TRACE ("OBJVAR: [%u]\n\n", objVar.GetCounter());

        //sleep (5);

        strData.clear();
    }
    
    ros_result_free(result);
}


void PrintWelcomeMessage ()
{
    Term.ResetAttr();
    Term.BigChar();
    Term.ColorBright();
    printf ("Mikrotik Monitoring and Management.\n");
    Term.ColorNormal();
    printf ("by Gustavo Campos, 2014.\n\n");
}



void LoadingHostTranslation ()
{
    FILE* pFile = NULL;

    if ((pFile = fopen ("/etc/MktHostTranslation.list", "r")) == NULL)
    {
        YYTRACE ("No /etc/MktHostTranslation.list found.")
        return;
    }

    char szData [1024];
    char szTemp [1024];
    string strHost, strValue;

    while (feof (pFile) == false)
    {
        fgets (szData, sizeof (szData) - 1, pFile);
        TUtil_StripEOL (szData, (int) strlen (szData));

        //TRACE ("Read: [%s]\n", szData);

        TUtil_GetToken (';', (const char*) szData, szTemp, sizeof (szTemp) - 1, 0);
        strHost = szTemp;

        TUtil_GetToken (';', (const char*) szData, szTemp, sizeof (szTemp) - 1, 1);
        strValue = szTemp;

        TRACE ("HOST: [%s] = [%s]\n", strHost.c_str(), strValue.c_str());

        mapHostDictionary [strHost] = strValue;
    }

    fclose (pFile);
}







int main(int nArgs, char *pszArgs [])
{

    whois objWhois;

    objWhois.SetCacheTimeout (10);


    string strData = "201.17.30.152";

    /*
    try
    {
        TRACE ("Answer: [%s]\n\n", objWhois.Lookup (strData.c_str(), strData.length()) == true ? "TRUE":"FALSE");

        TRACE ("Owner:   [%s]\n", objWhois ["owner"]);
        TRACE ("Contact: [%s]\n", objWhois ["person"]);
        TRACE ("Country: [%s]\n", objWhois ["country"]);
        
    } catch (Exception* pEx)
    {
        YYTRACE ("Error: [%s]\n", pEx->GetExceptionMessage());
    }
*/

    //exit (0);

    PrintWelcomeMessage ();
    
    if (nArgs != 8)
    {
        Term.ColorBright();
        printf ("(%u)Use: <Router IP> <Router port - nothig is default> <Router user> <Router user password> <Transnitting Kbps Threshold> <Receiving Kbps Threshold> <Router Interface>\n\n", nArgs);
        Term.ResetAttr();
        
        exit (1);
    }

    LoadingHostTranslation ();

    Term.GetTermSize (&nColumns, &nLines);

    ServerAttr.nTXLimit = strtod (pszArgs [5], NULL);
    ServerAttr.nRXLimit = strtod (pszArgs [6], NULL);
    ServerAttr.pszInterface = pszArgs [7];
    ServerAttr.pszRouterAddress = pszArgs [1];
    
    class MikrotikLib pRouter (pszArgs [1], pszArgs [2][0] == '\0' ? 8728 : atoi (pszArgs [2]), pszArgs [3], pszArgs [4]);

    /*
    pRouter.SendCommand ("/interface/print  ", handledata, 1);

    pRouter.SendCommand ("/interface/monitor-traffic =interface=bridge-local", handledata, 100);
    */
    
    //pRouter.SendCommand ("/ip/dhcp-server/lease/print ", IPNameStructure, 101);
   


    string strInterfaceMonitorCmd = "";

    ServerAttr.bHealthSupport = true;


    strIPInterfaceMonitoring = "";
    strIPInterfaceMonitoring = strIPInterfaceMonitoring + "/tool/torch =dst-address=0.0.0.0/0 =freeze-frame-interval=4 =interface=" + pszArgs [7] + " =src-address=0.0.0.0/0 =ip-protocol=tcp ";

    string strTouchString;

    strTouchString = strTouchString + "/tool/torch =src-address=0.0.0.0/0 =interface=" + pszArgs [7] + "  =freeze-frame-interval=5 ";


    map<string, struct IPUsage*>::iterator it;

    pHostList.clear();

    Term.Color (0, 7, 0);
    Term.Cls ();

    Term.GetTermSize (&nColumns, &nLines);

    //pRouter.SetDebug (true);


    map<string, struct Interfaces*>::iterator mapInterfacei;


    TRACE ("STARTING....\n\n");

    while (true)
    {
        //Term.SetScroll (nLines - 10, nLines);
        pRouter.ProcessLoopOnce();
        //Term.EndScroll ();

        
        if (bIPMonitoringIP == false)
        {
            pRouter.SendCommand ((const char*) strTouchString.c_str(), IPUsageHandle , 200);
            bIPMonitoringIP = true;
        }


        if (bIPMonitoringConnected == false)
        {
            pRouter.SendCommand (strIPInterfaceMonitoring.c_str(), IPMonitoringHandler, 150);
            bIPMonitoringConnected = true;
        }

        
        if(bProfileMonitor == false)
        {
             pRouter.SendCommand ("/tool/profile =freeze-frame-interval=5", ProfileHandler, 106);
            bProfileMonitor = true;
        }

        pRouter.SendCommand ("/ip/dhcp-server/lease/print ", IPNameStructure, 101);

        pRouter.SendCommand ("/system/resource/print ", ResourceHandler, 105);

        pRouter.SendCommand ("/system/health/print ", HealthHandler, 110);

        pRouter.SendCommand ("/interface/print  ", InterfacesDiscoverHandler, 210);

        strInterfaceMonitorCmd = "/interface/monitor-traffic =interface=";

        for (mapInterfacei = mapInterfaces.begin(); mapInterfacei != mapInterfaces.end(); mapInterfacei++)
        {
            strInterfaceMonitorCmd = strInterfaceMonitorCmd + (*mapInterfacei).second->strName + ",";
        }

        pRouter.SendCommand (strInterfaceMonitorCmd.c_str(), InterfaceDataHandler, 220);

        //TRACE ("Processando: [%s]\n", strInterfaceMonitorCmd.c_str());


        Render ();
        fflush (stdout);


        sleep (5);

        Term.ResetAttr();
        Term.Cls ();

        pRouter.SendCommand ("/cancel =tag=220 ", NullHandler, 22);
        Term.ResetAttr();

    }
    
	return 0;
}

