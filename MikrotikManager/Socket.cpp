/*
 *  Socket.cpp
 *  thread
 *
 *  Created by Gustavo Campos on 16/04/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <sys/socket.h>
#include "Util.h"
#include "Exception.h"
#include <iostream>
#include "Socket.h"
#include <stdlib.h>
#include <stdio.h>
#include <openssl/rand.h>

static void sigpipe_handle(int x);

static char*		pass;


void Socket::DefaultInicialization ()
{
	nDomain = 0;
    nType =0;
    nSocket = 0;
    isListen = 0;
    isConnected = 0;
	pFile = NULL;
	
	bio_err = NULL;	
	
	bSecure = false;
		
    ctx = NULL;
    ssl = NULL;
    sbio = NULL;
	
}



Socket::~Socket ()
{
	//if (isListen == false)
    Close ();
}



Socket::Socket ()
{
	DefaultInicialization ();
	
 #ifdef WIN32
    static bool bWStart = false;
    
    if (bWStart == false)
    {
        WSADATA wsaData;
        WSAStartup(0x0101, &wsaData); /* Winsock Startup function. DEFAULT... to all source files that will use Winsock **only WIN32  */
        bWStart = true;
    }

	pFile = NULL;

    nLastReading = 0;
    nLastWritting = 0;

#endif
}




bool Socket::IsReady (int nTimeoutSeconds, int nTimeoutUSeconds)
{
    fd_set            fdSet;
    struct timeval    tmVal;
    bool              bReturn;
    unsigned int      nReturn;

	VERIFY (this != NULL, "DEEP LOGIC ERROR... COMPILING ERROR.");
	VERIFY (nSocket _ERROR, "Error, Socket not defined.");

    tmVal.tv_sec  = nTimeoutSeconds;
    tmVal.tv_usec = nTimeoutUSeconds;
    
    FD_ZERO (& fdSet);
    FD_SET  (this->nSocket, &fdSet);
        
    
#ifndef _HPUX_
    VERIFY ((nReturn = select (this->nSocket + 1, &fdSet, NULL, NULL, &tmVal)) _ERROR, "");
#else
    VERIFY ((nReturn = select (this->nSocket + 1, &fdSet, NULL, NULL, &tmVal)) _ERROR, ""); 
#endif

    VERIFY ((nReturn = FD_ISSET (this->nSocket, &fdSet)) >= 0, "");
    
	NOTRACE ("%s: Result: %u  %us %us\n", __FUNCTION__, nReturn, nTimeoutSeconds, nTimeoutUSeconds);
	
    if (nReturn > 0)
    {
        bReturn = true;
    }
    else
    {
        bReturn = false;
    }

    return bReturn;
}




bool Socket::IsWriteReady (int nTimeoutSeconds, int nTimeoutUSeconds)
{
    fd_set            fdSet;
    struct timeval    tmVal;
    bool              bReturn;
    int               nReturn;
     
    tmVal.tv_sec  = nTimeoutSeconds;
    tmVal.tv_usec = nTimeoutUSeconds;
    
	Assert  (this != NULL, "DEEP LOGIC ERROR... COMPILING OR LOGIC ERROR.", 100);
	VERIFY (nSocket _ERROR, "Error, Socket not defined.");
	
    FD_ZERO (& fdSet);

    FD_SET  (nSocket, &fdSet);
        


#ifndef _HPUX_
    VERIFY (select (this->nSocket + 1, NULL, &fdSet, NULL, &tmVal) > 0, "");
#else
    VERIFY (select (this->nSocket + 1, NULL, (int*) &fdSet, NULL, &tmVal) > 0, "");
#endif

    VERIFY ((nReturn = FD_ISSET (this->nSocket, &fdSet)) _ERROR, "");


    if (nReturn)
    {
        bReturn = true;
    }
    else
    {
        bReturn = false;
    }

    return bReturn;
}





bool Socket::Close ()
{
    /* 
       For better performance and compatibilitie this
       proceadure is totally safety, throwing no warnnings
    */
    
    //VERIFY (this->isListen != true, "Socket was defined as LISTEN and could not be closed.");
    
		/* Disabilita send and receive */
		shutdown  (this->nSocket, SHUT_RDWR); 
		
		/* Close connection with the remote peer*/
		close (this->nSocket);

    isListen = false;
    isConnected = false;

    return true;
}




bool Socket::Printf (long int nStrLen, const char* pszFormat, ...)
{
        va_list   vaArgs;
        char      pszTemp [nStrLen + 1];
        long int nLen;
        
        //signal (SIGPIPE, Socket::PipeReceive);
        
        if (nStrLen > SOCKET_MAX_PRINTF_LINE) nStrLen = SOCKET_MAX_PRINTF_LINE;
                
        va_start (vaArgs, pszFormat);
        
        VERIFY ((nLen = vsnprintf ((char*) pszTemp, nStrLen-1, pszFormat, vaArgs)) _ERROR, "Erro ao processar a string a ser enviada.");
        
        pszTemp [nLen] ='\0';
        
        va_end (vaArgs);
        
		TRACE ("PRINTF: (%u) %s\n", nLen, pszTemp);
		
		Send (nLen, pszTemp);

        nLastWritting = time (NULL);

        return true;
}




bool Socket::Send (long int nStrLen, const char* pszData)
{        
        //signal (SIGPIPE, Socket::PipeReceive);


        NOTRACE ("Sending: %s\n", pszData);
        
		VERIFY (IsWriteReady (15, 100), "Timeout de envio.");
		
        VERIFY (send (this->nSocket, (char*) pszData, nStrLen, 0) _ERROR, "Erro ao Enviar os dados ao Socket destino");

        nLastWritting = time (NULL);
    
        return true;
}




long int Socket::Read (int nStrLen, char* pszReceive)
{
    return Read (nStrLen, pszReceive, 10);
}




long int Socket::Read (int nStrLen, char* pszReceive, uint nTimeout)
{
    long int nLen;
    
    if (IsReady (nTimeout, 0) == true)
    {
        try
        {
            VERIFY ((nLen = recv (this->nSocket, pszReceive, nStrLen, MSG_WAITALL)) > 0, strerror (errno));
        }
        catch (Socket* pSocket)
        {
            pSocket->Close ();
            throw;
        }
    }
    else
    {
        return 0;
    }
    
    NOTRACE ("Socket::SafeRead: Readed %ld \n", nLen);

    nLastReading = time (NULL);

    return nLen;
}




long int Socket::Gets (char* pszReturn, long int nLen, long int nTimeout)
{
    long int nCount     = 0;
            
    pszReturn [0] = '\0';
    
    if (IsReady ((int) nTimeout, 0))
    {
        char     chChar [2] = " ";
        long     nReceive;

        while (chChar [0] != '\n')
        {
            if (IsReady  ((int) nTimeout, 0))
            {
				if ((nReceive = Read (1, &chChar[0])) != 1)
                {
                    //TRACE ("ERRNO: [%lu]\n", nReceive);
                    pszReturn [nCount] = _EOS;                    
                }
            }
            else
            {
                break;
            }
            
            
            /* 
            A cada 10 ciclo de processamento e direcionado uma 
             pausa de 10 mile segundos 
             */
                        
            if (chChar [0] >= ' ')
            {
                if (nCount < nLen - 1)
                {
                    pszReturn [nCount] = chChar [0];
                    nCount++;
                    pszReturn [nCount] = _EOS;
                }
                else
                {
                    pszReturn [nCount] = _EOS;
					//TRACE ("GETS:  %s\n", pszReturn);
                    return nCount;
                }
            }
        }        
    }
    else
    {
        return 0;
    }
    
    /* Retorna o dado encontrado */ 
    pszReturn [nCount] = _EOS;
	//TRACE ("GETS:  %s\n", pszReturn);

    return nCount;
}




bool Socket::Listen (int nPorta, int nBacklog)
{
    int nTrue = 1;
    struct addrinfo hints, *res;
	int nRet;
	struct timeval tmVal; 
	char   szPorta [16];
		    
	tmVal.tv_sec = 10;
	tmVal.tv_usec = 900000;
	
	snprintf (szPorta, 15, "%u", nPorta);
	
	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	 
	if ((nRet = getaddrinfo ("0.0.0.0", szPorta, &hints, &res)) != 0)
	{
		NOTRACE ("TCP Listen Error: %s\n", gai_strerror (nRet));
		return false;
	}
		
	VERIFY ((this->nSocket = socket (res->ai_family, res->ai_socktype, 0)) _ERROR, "");
    
    VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_REUSEADDR, (int*) &nTrue, sizeof (int)) _ERROR, "");

    VERIFY (setsockopt (this->nSocket, IPPROTO_TCP, TCP_NODELAY, &nTrue, sizeof (int)) _ERROR, "");

    //VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_ERROR, &nTrue, sizeof (int)) _ERROR, "");

    VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_SNDTIMEO, &tmVal, sizeof (struct timeval)) _ERROR, "");
    //VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_SNDBUF, &nTrue, sizeof (int)) _ERROR, "", false);

    int set = 1;

#ifdef SO_NOSIGPIPE
    VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)) _ERROR, "");
#endif

    nTrue = 10;
    
    VERIFY ((bind (this->nSocket, res->ai_addr, res->ai_addrlen)) >= 0, "");
    
    this->isListen = true;

    VERIFY (listen (this->nSocket, nBacklog) _ERROR, "");

    this->nType   = SOCK_STREAM; 
    this->nDomain = AF_INET;
    
    return true;
}





Socket* Socket::Accept (uint nTimeout)
{
    Socket*      pNSocket = NULL;
    int          nSocket;
    sockaddr_in  SocketAddr;
    socklen_t    sin_size = sizeof (struct sockaddr);
    
    VERIFY (this->isListen == true, "O Socket n„o est· em modo Listen");
 
    VERIFY (this->nDomain == AF_INET && this->nType == SOCK_STREAM, "Tipo de socket invalido.");
    
    if (IsReady (nTimeout, 0) == false)
    {
        return NULL;
    }
    
    try
    {		
        VERIFY ((nSocket = accept(this->nSocket, (struct sockaddr *) &SocketAddr, &sin_size)) _ERROR, "");
        
        //TRACE ("[%s] conectado FD: [%d] \n", inet_ntoa (SocketAddr.sin_addr), nSocket);
        
        VERIFY ((pNSocket = new Socket (nSocket, SocketAddr)) != NULL, "Erro na criacao do socket de conexao.");
        
    }
    catch (Socket* pSocket)
    {
        NOTRACE ("%s\n", pSocket->GetExceptionMessage ());
        
        if (pNSocket != NULL)
        {
            delete pNSocket;
        }
		
		throw pSocket;
    }
    
    return pNSocket; 
}




Socket::Socket (int nSocket, sockaddr_in stSocketAddr)
{        
	
	DefaultInicialization ();
	
		{
			int window = 1024  * 1;  // 1K
			int  nNDelay = 1;

            this->nSocket = nSocket;

            int nLen;

			//VERIFY (setsockopt (this->nSocket, IPPROTO_TCP, TCP_NODELAY, &nNDelay, sizeof (int)) _ERROR, "");
			//VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_DONTROUTE, &nNDelay, sizeof (int)) _ERROR, "", false);

            nLen = 20048;
			VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_RCVBUF, &nLen, sizeof (nLen)) _ERROR, "");

            nLen = 20048;
			VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_SNDBUF, &nLen, sizeof (nLen)) _ERROR, "");

#ifdef SO_NOSIGPIPE
            int set = 1;
            VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)) _ERROR, "");
#endif

            {
                struct timeval tv;

                tv.tv_sec = 30;  /* 30 Secs Timeout */

                setsockopt(this->nSocket, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

                setsockopt(this->nSocket, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

                int optval;
                socklen_t optlen = sizeof(optval);

                optval = 10;
                optlen = sizeof(optval);

                VERIFY (setsockopt(this->nSocket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) >= 0, "", false);
            }

		}

        memcpy (&this->SocketAddr, &stSocketAddr, sizeof (sockaddr_in));

    isConnected = true;

    nLastReading = time (NULL);
    nLastWritting = time (NULL);

}




bool Socket::Connect (const char* pszHost, unsigned int nPort, int nTimeout)
{
	return Connect2 (pszHost, nPort, nTimeout, false);
}




bool Socket::SSLConnect (const char* pszHost, unsigned int nPort, int nTimeout)
{
	return Connect2 (pszHost, nPort, nTimeout, true);
}




bool Socket::Connect2 (const char* pszHost, unsigned int nPort, int nTimeout, bool bSSLCom)
{  
    struct hostent* pHostEnt;
	
    static int s_server_session_id_context = 0;

    s_server_session_id_context++;

    //Requisita um novo socket ao sistema operacioal
	VERIFY ((this->nSocket = socket(AF_INET, SOCK_STREAM, 0)) _ERROR, "");

	bSecure = bSSLCom;
	
    //Converte o DNS / Host entrado. (realize o DNS)
    pHostEnt = GetHostByName (pszHost);
  
    memset(&this->SocketAddr, 0, sizeof(struct sockaddr_in));
    this->SocketAddr.sin_family = AF_INET;
    this->SocketAddr.sin_addr.s_addr = ((struct in_addr*)(pHostEnt->h_addr))->s_addr;
    this->SocketAddr.sin_port = htons(nPort);                                                                   

    int nLen = 20048;
    VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_RCVBUF, &nLen, sizeof (nLen)) _ERROR, "");

    nLen = 20048;
    VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_SNDBUF, &nLen, sizeof (nLen)) _ERROR, "");


    {
        struct timeval tv;

        tv.tv_sec = nTimeout;  /* 30 Secs Timeout */

        setsockopt(this->nSocket, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

        setsockopt(this->nSocket, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

        /* Set the option active */

        int optval;
        socklen_t optlen = sizeof(optval);

        optval = 10;
        optlen = sizeof(optval);

        VERIFY (setsockopt(this->nSocket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) >= 0, "", false);
    }



    //Connecta ao host destino
    VERIFY (connect(this->nSocket,(struct sockaddr *)  &this->SocketAddr, sizeof(struct sockaddr)) _ERROR, "Erro ao se conectar ao host destino");    
	
	{
		//int window = 1024 * 1;  // 1.0M
		//VERIFY (setsockopt (this->nSocket, SOL_SOCKET, SO_RCVBUF, &window, sizeof (window)) _ERROR, "")
	}
	


    nLastReading = time (NULL);
    nLastWritting = time (NULL);

	return true;
}




struct hostent* Socket::GetHostByName (const char* pszHost)
{
  struct hostent* pHostEnt;
  
  //TRACE ("Procurando host: %s\n", pszHost);

  VERIFY ((pHostEnt = gethostbyname (pszHost)) != NULL, "O DNS entrado nao foi encontrado.\n");

  return pHostEnt;
}




FILE* Socket::GetNewFILE (char* szFileMode)
{	
	VERIFY ((pFile = fdopen (nSocket, szFileMode)) != NULL, "");
	
	return pFile;
}




void Socket::GetLocalAddress (struct sockaddr_in* cliaddr)
{ 
        int     sockfd; 
        socklen_t len; 
        struct sockaddr_in servaddr; 
        
        sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
        bzero(&servaddr, sizeof(servaddr)); 
        
        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(1); 
        
        inet_pton(AF_INET, "255.255.255.255", &servaddr.sin_addr); 
        
        connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)); 
        
        len = sizeof(cliaddr); 
        getsockname(sockfd, (struct sockaddr *) cliaddr, &len); 
		
        //NOTRACE ("local address %s\n", inet_ntoa (cliaddr->sin_addr));
} 




void Socket::IfException ()
{
	//NOTRACE ("EXCEPTION RISED FROM SOCKET\n");
}






/* A simple error and exit routine*/
int err_exit(char* string)
{
    fprintf(stderr,"%s\n",string);
    exit(0);
}




static void sigpipe_handle(int x)
{
}



#define GetIP(x) inet_ntoa (x.sin_addr)
#define GetPort(x) ntohs (x.sin_port)

const char* Socket::GetRemoteIP ()
{
    if (isConnected == true)
    {
        return inet_ntoa (this->SocketAddr.sin_addr);
    }

    return NULL;
}




uint16_t Socket::GetRemotePort ()
{
    if (isConnected == true)
    {
        return ntohs (this->SocketAddr.sin_port);
    }

    return 0;
}




int Socket::GetSockfd()
{
    return nSocket;
}




uint Socket::GetFdSetFromArray (Socket** pSockets, uint nMaxSockets, fd_set* objFdSet, uint* nMaxFd)
{
    Verify (pSockets != NULL, "Error no data stored over pSocket pointer variable.", false);
    Verify (objFdSet != NULL, "Error no fd_set data provided, the variable is NULL.", false);

    FD_ZERO(objFdSet);

    uint nCount;
    uint nFDCount=0;

    *nMaxFd = 0;

    for (nCount=0; nCount < nMaxSockets; nCount++)
    {
        if (pSockets [nCount] != NULL)
        {
            //TRACE ("Adding [%u] FD:[%u] to the objFDSet given at [%08X]\n", nCount, pSockets [nCount]->GetSockfd(), objFdSet);
            FD_SET (pSockets [nCount]->GetSockfd(), objFdSet);
            nFDCount++;

            if (pSockets [nCount]->GetSockfd() > *nMaxFd) *nMaxFd = pSockets [nCount]->GetSockfd();
        }
        else
        {
            //YYTRACE ("FdSet: Socket Index [%u] is NULL\n", nCount);
        }
    }

    //TRACE ("\n");
    //TRACE ("\n");

    return nFDCount;
}




/*
   CheckDataForReading ()
 
    This function will return how much data ready for being read
    from the current connection, if if return zero, it may means 
    the socket has gone disconnected but yet undetected from the current
    functions.
 */

uint32_t Socket::CheckDataForReading ()
{
    int n = 0;

    ioctl(nSocket, FIONREAD, &n);
    
    return n;
}



/*
    CheckIfIsClosed ()
 
    This function will get the amount of data on the Kernel's current Socket's pipeline
    and case it is zero will return that the connection is already broken. but 
    will not take action to close it, just to inform.
 
    Even though it may look a non sence, it will give to the programmer
    the hability to proper act before closing the connection, what is highly wised
    and good-practive. 
 
    For closing use ->close () directive whereabouts it is ready in your code.
 */

bool Socket::CheckIfIsClosed ()
{
    int nError = 0;
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(nSocket, &rfd);

    timeval tv = { 0 };


    char chData;


    VERIFY (select(nSocket+1, &rfd, NULL, NULL, &tv) >= 0, "", true);

    if (FD_ISSET (nSocket, &rfd))
    {
        int l_sock_optval=-1;
        int l_sock_optval_len=sizeof(l_sock_optval);

        if(getsockopt(nSocket, SOL_SOCKET, SO_ERROR, (int*)&l_sock_optval, (socklen_t*)&l_sock_optval_len) !=0)
        {
            return true;
        }
    }

    int n = 0;

    ioctl(nSocket, FIONREAD, &n);

    return n >= 0 ? false : true;
}



/*
    CheckReadingIdelTimeout (uint nSecTimeout)
 
   Will return true if the elipsed time between last reading 
   to now is greatter than nSecTimeout.
 
   This functions only say if the give timout has been reached, it
   will not take any action, this in inteded to allow programmer
   to logical enrich applications.
*/

bool Socket::CheckReadingIdelTimeout (uint nSecTimeout)
{
    time_t nCurrentTime = time (NULL);

    //TRACE ("Last reading TS: [%lu] Now TS: [%lu] Difference: [%lu] threshold: [%u]\n", nLastReading, nCurrentTime, nCurrentTime - nLastReading, nSecTimeout);

    if ((nCurrentTime - nLastReading) >= nSecTimeout)
        return true;

    return false;
}


/*
 CheckWritingIdelTimeout (uint nSecTimeout)

 Will return true if the elipsed time between last writing
 to now is greatter than nSecTimeout.

 This functions only say if the give timout has been reached, it
 will not take any action, this in inteded to allow programmer
 to logical enrich applications.
 */


bool Socket::CheckWritingIdelTimeout (uint nSecTimeout)
{
    time_t nCurrentTime = time (NULL);

    //TRACE ("Last Writting TS: [%lu] Now TS: [%lu] Difference: [%lu] threshold: [%u]\n", nLastWritting, nCurrentTime, nCurrentTime - nLastWritting, nSecTimeout);

    if ((nCurrentTime - nLastWritting) >= nSecTimeout)
        return true;

    return false;
}



/*
 CheckIdelTimeout (uint nSecTimeout)

 Will return true if the elipsed time between last writing or reading
 to now is greatter than nSecTimeout.

 This functions only say if the give timout has been reached, it
 will not take any action, this in inteded to allow programmer
 to logical enrich applications.
 */

bool Socket::CheckIdelTimeout (uint nSecTimeout)
{
    if (CheckReadingIdelTimeout(nSecTimeout))
    {
        YYTRACE ("Reading Timed out.\n");

        return true;
    }
    else if (CheckWritingIdelTimeout(nSecTimeout))
    {
        YYTRACE ("Writing Timed out.\n");

        return true;
    }

    return false;
}

