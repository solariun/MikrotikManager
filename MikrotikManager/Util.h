/*
 *  Util.h
 *  thread
 *
 *  Created by Gustavo Campos on 21/04/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
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



#ifndef UTIL_H_FRAMEWORK
#define UTIL_H_FRAMEWORK


#include <Exception.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

using namespace std;


#define Container //Creating a container

#define SymbolName(x) #x

#ifndef WIN32
#include <pthread.h>
#define UINT unsigned int
#else



//#define _WIN32_WINNT  0x0501
//#include <winsock2.h>
#include <windows.h>
//#pragma comment(lib,"wsock32")
#define socklen_t size_t
//#define ssize_t size_t

#define bzero(point,size) memset (point, 0, size)

#define sleep Sleep

#define usleep(x) sleep (x / 1000)

#define _LITTLE_ENDIAN


#define MAXPATHLEN      _MAX_PATH       /*!< Maximum length of full path. */
#define IOV_MAX         16              /*!< Max # of iovec structures for readv/writev. */

#define STDIN_FILENO    0               /*!< Valor padr<E3>o para fileno (stdin). */
#define STDOUT_FILENO   1               /*!< Valor padr<E3>o para fileno (stdout). */
#define STDERR_FILENO   2               /*!< Valor padr<E3>o para fileno (stderr). */

#define SIGKILL         9               /*!< kill (cannot be caught of ignored). */
#define SIGPIPE         13              /*!< write on a pipe with no one to read it. */
#define SIGALRM         14              /*!< alarm clock. */
#define SIGHUP          1               /*!< hung up. */

#define ENOMSG          35              /*!< No message of desired type. */
#define EIDRM           36              /*!< Identifier removed. */
#define ETIME           52              /*!< Timer expired. */

#define WNOHANG         1               /*!< don't hang in wait. */
#define WUNTRACED       2               /*!< tell about stopped, untraced children. */

#ifndef WIN32
#define S_IRWXU         00700           /*!< read, write, execute: owner. */
#define S_IRUSR         00400           /*!< read permission: owner. */
#define S_IWUSR         00200           /*!< write permission: owner. */
#define S_IXUSR         00100           /*!< execute permission: owner. */
#define S_IRWXG         00000           /*!< read, write, execute: group. */
#define S_IRGRP         00000           /*!< read permission: group. */
#define S_IWGRP         00000           /*!< write permission: group. */
#define S_IXGRP         00000           /*!< execute permission: group. */
#define S_IRWXO         00000           /*!< read, write, execute: other. */
#define S_IROTH         00000           /*!< read permission: other. */
#define S_IWOTH         00000           /*!< write permission: other. */
#define S_IXOTH         00000           /*!< execute permission: other. */
#endif

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <setjmp.h>

#ifndef WIN32
#include <alloca.h>
#else

#define dev_t      _dev_t
#define ino_t      _ino_t
#define mode_t     _mode_t
#define off_t      _off_t

#define lstat  stat
#define S_ISLNK(x) x == true? false : false  


#endif


#ifndef _NOMD5

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;



extern "C"
{
    
    /* Initialize the algorithm. */
    void md5_init(md5_state_t *pms);
    
    /* Append a string to the message. */
    bool md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
    
    /* Finish the message and return the digest. */
    bool md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
    
}  /* end extern "C" */

#endif


#define UTIL_B64ENCLEN(x) ((int)((int)((x%3) == 0 ? 0 : 1) + x / 3) * 4)

//#define UTIL_B64ENCLEN(x) ( (double)((int)(x/3)) < (double)(x/3) ? ((int)((x/3)) + 1) * 4 : 10000 )

#define	LLUINT long long unsigned int 
#define	LUINT  long unsigned int      
#define	UINT   unsigned int           

typedef unsigned int uint;

#define Countof(x) (sizeof (x) - 1)

#define NOT !

#define UTIL_FIELDLEN   128


#define PRIVATE 


#define _EOS '\0'

#define ON  1
#define OFF 0

#define _ERROR >= 0
	
#define IsBitOn(x,y) ((x & y) != 0 ? true : false)
#define ISBITON(Mask, Value) (Value & Mask)


#define HexChar(x) (x > 9 ? (x - 10) + 'A' : x + '0')
#define HexVal(x)  (x >= '0' && x <= '9' ? x - '0' : (x >= 'a' && x <= 'z') ? x - 'a' + 10 : x - 'A' + 10)

#ifdef OLDCRC
  #define POLY64REV	0xd800000000000000ULL
  #define INITIALCRC	0x0000000000000000ULL
#else
  #define POLY64REV     0x95AC9329AC4BC9B5ULL
  #define INITIALCRC    0xFFFFFFFFFFFFFFFFULL
#endif



#define Util_GetIP(x) inet_ntoa (x.sin_addr)
#define Util_GetPort(x) ntohs (x.sin_port)


void TUtil_StripEOL (char* pszString, int nLen);
bool TUtil_TrimString (char* pszString, int nLen);
bool TUtil_ReplaceChar (char* pszString, int nLen, char chCharIn, char chCharOut);
bool TUtil_ValidateMask (const char* pszMask, const char* pszString, int nLen);
uint32_t TUtil_CountToken (const char chToken, const char* pszString,uint32_t nStrLen);
char* TUtil_GetToken (char chToken, const char* pszString, char* pszReturn, long int nStrLen, int nToken);
bool TUtil_Crypt (unsigned char* pszKey, unsigned char* pszData, size_t nSize);
bool TUtil_GenerateRandomKey (char* pszKey, int nLen);
ssize_t TUtil_WriteCript (char* pszKey, int nFd, void* pData, size_t nSize);
ssize_t TUtil_ReadCript (char* pszKey, int nFd, void* pData, size_t nSize);
bool TUtil_Uppcase (char* pszString);
bool TUtil_GetValueFromResponse (const char* pszSource, const char* pszField, char* pszData, int nDataLen);
const char* TUtil_GetPtrFromToken (char chToken, const char* pszText, uint32_t nTextSize, uint16_t nToken);

bool TUtil_GetValueFromResponse2 (const char* pszSource, const char* pszField, char* pszData, int nDataLen, char chDelimiter, char chDataDv);
bool TUtil_GetValueFromResponse3 (const char* pszSource, const char* pszField, char* pszData, int nDataLen, char chDelimiter, char chDataDv);
bool TUtil_TokennedUppcase (char* pszString, char szToken);
void TUtil_NanoSleep (long int nTime, long int nTime2);
bool TUtil_MD5hex (const char *src, char *hex_output);
uint64_t TUtiL_crc64(const char *seq);
uint64_t TUtiL_crc64_v2(const char *seq, uint64_t crc);

uint64_t TUtiL_crc64Binary (const char *seq, long int nLen, uint64_t crc);
uint64_t TUtiL_crc64Base2Binary (const char *seq, long int nLen, uint64_t crc);
uint64_t TUtiL_crc64Base3Binary (const char *seq, long int nLen, uint64_t crc);

uint32_t TUtil_CRC32(const uint8_t *data, size_t n_bytes, uint32_t crc);

bool TUtil_IsReady (int nFD, int nTimeoutSeconds, int nTimeoutUSeconds);

bool Util_IPMaskValidating (char* pszIP, char* pszMask);
// bool Util_Base64Encode (uint8_t* pszDataRaw, uint8_t* pszReturnBase64);
// bool Util_Base64Decode (uint8_t* pszDataRaw, uint8_t* pszReturnBase64);

long int Util_EncodeToBase64 (const char* pszString, const long int nStrLen, char* pszRetString, const long int nRetStringMaxLen);
long int Util_DecodeFromBase64 (const char* pszBase64String, const long int nB64StrLen, char* pszRetString, const long int nRetStringMaxLen);

int Util_MatrixCode2of5 (const char* pszBarCode, char* pszReturnBuffer, long int nBufferLen);

#ifndef WIN32
void Util_CPU_Init ();
void Util_CPUReduce ();
double Util_CPU_Usage ();
double Util_CPU_UsagebyChild ();
double Util_CPU_GetUsage (bool bChild);

void Util_SetMaxCPU (unsigned long int nMax);
#endif

clock_t Util_GetClock (void);

uint32_t Util_CRC32v2 (unsigned char *block, unsigned int length, uint32_t crc_start);

void UTil_PrintDataToDebug (uint8_t* szSectionData, long int nDataLen);

void Util_PrintBinary (uint8_t chData);

bool Util_IsLittleEndian ();
uint32_t Util_ReverseEndian (uint32_t nData);

char* TUtil_GenerateUID (const char* pszData, uint32_t nDataSize, char* pszReturn, uint32_t nRetLen);
uint64_t TUtil_GenerateAssign (const char* pszData, uint32_t nDataSize, char* pszReturn, uint32_t nRetLen);

bool TUtil_AssignString (string& strData);
bool TUtil_CheckAssignedString (const char* pszData, uint32_t nLength);

double Util_Show_CPU_Usage (bool bChild);

#endif

