/*
 *  Socket.h
 *  thread
 *
 *  Created by Gustavo Campos on 16/04/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SOCKET_H_CPP
#define SOCKET_H_CPP

#ifdef WIN32
#include <wininet.h>
#include <ws2tcpip.h>
#define SHUT_RDWR SD_BOTH 
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#include <Exception.h>

#define CA_LIST "root.pem"
#define HOST	"localhost"
#define RANDOM  "random.pem"
#define PORT	4433
#define BUFSIZZ 1024

#define KEYFILE "client.pem"
#define PASSWORD "MSNCpp.P9"


#ifndef ALLOW_OLD_VERSIONS
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
#error "Must use OpenSSL 0.9.6 or later"
#endif
#endif


#define SOCKET_EXCEPTION_MESSAGE_SIZE		312
#define SOCKET_MAX_PRINTF_LINE              2048

class Socket : public Exception
{
private:
    int                   nDomain;
    int                   nType;
    int                   nSocket;
    sockaddr_in           SocketAddr;
    bool                  isListen;
    bool                  isConnected;
	FILE*				  pFile;

    time_t                nLastReading;
    time_t                nLastWritting;
    
	BIO*				  bio_err;	
	
	bool				  bSecure;

	void        destroy_ctx(SSL_CTX *ctx);
	SSL_CTX*    initialize_ctx(char* keyfile, char* password);
	void		DefaultInicialization ();
	
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *sbio;


protected:
/* Exception selfspect */
	
	Socket (int nSocket, sockaddr_in stSoketAddr);
	void IfException ();
	
public:
		
	Socket ();
	~Socket ();
	
	FILE* GetNewFILE (char* szFileMode);

	bool IsReady (int nTimeoutSeconds, int nTimeoutUSeconds);
	bool IsWriteReady (int nTimeoutSeconds, int nTimeoutUSeconds);
	bool Close ();
	bool Printf (long int nStrLen, const char* pszFormat, ...);
	bool Send (long int nStrLen, const char* pszData);
	long int Read (int nStrLen, char* pszReceive, uint nTimeout);
	long int Read (int nStrLen, char* pszReceive);
	long int Gets (char* pszReturn, long int nLen, long int nTimeout);
	
	bool Connect (const char* pszHost, unsigned int nPort, int nTimeout);
	bool SSLConnect (const char* pszHost, unsigned int nPort, int nTimeout);
	bool Connect2 (const char* pszHost, unsigned int nPort, int nTimeout, bool bSSLCom);

	Socket* Accept (uint nTimeout);
	
	bool Listen (int nPorta, int nBacklog);
	void GetLocalAddress (struct sockaddr_in* cliaddr);
	bool IsItConnected () { return isConnected; };
    bool IsItaListen () { return isListen; };

    uint32_t CheckDataForReading (); //This will return how much data to read on the Kernel's socket pipe. if it return zero, means connection is closed.

    bool CheckIfIsClosed (); //This will return if the current connection is closed.

	struct hostent* GetHostByName (const char* pszHost);

    const char* GetRemoteIP ();
    uint16_t GetRemotePort ();

    int GetSockfd ();
    
    static uint GetFdSetFromArray (Socket** pSockets, uint nMaxSockets, fd_set* objFdSet, uint* nMaxFd);

    bool CheckReadingIdelTimeout (uint nSecTimeout);
    bool CheckWritingIdelTimeout (uint nSecTimeout);

    bool CheckIdelTimeout (uint nSecTimeout);
};


#endif
