/*
 *  Exception.h
 *  thread
 *
 *  Created by Gustavo Campos on 24/04/08.
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


#ifndef EXCEPTION_H_CPP
#define EXCEPTION_H_CPP 1


//#include <iostream.h>
#include <string.h>
#include <string>
#include <Exception.h>
#include <errno.h>

using namespace std;


#if defined _WIN32 || defined __CYGWIN__
#define _IMPORT __declspec(dllimport)
#define API_EXPORT __declspec(dllexport)
#define API_LOCAL
#else
#include <stdarg.h>
#if __GNUC__ >= 4
#define API_IMPORT __attribute__ ((visibility("default")))
#define API_EXPORT __attribute__ ((visibility("default")))
#define API_LOCAL  __attribute__ ((visibility("hidden")))
#else
#define API_IMPORT
#define API_EXPORT
#define API_LOCAL
#endif
#endif


#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

static string strGlobalStrError = "";

#define strLastException strGlobalStrError


#define YYTRACE(x,z...) {fprintf (stderr, "YMSG: %s(%u):", __FUNCTION__, __LINE__); fprintf (stderr, x, ##z);}

#ifdef _DEBUG
#define TRACE(x,z...) { fprintf (stderr, "DMSG: %s(%u):", __FUNCTION__, __LINE__); fprintf (stderr, x, ##z);}
#else
#define TRACE(x,z...) 1 ? (void) 0 : (void)fprintf (stderr, x, ##z)
#endif

#define NOTRACE(x,z...) 1 ? (void) 0 : (void)fprintf 

#define EXCEPTION_MESSAGE_SIZE		512
#define EXCEPTION_MESSAGE2_SIZE		320

#define NULL_VERIFY(x, y) Verify (x != NULL, "Obejto "#x" esta nulo.", y);

//ExceptionHandle (char* pszFile, char* pszFunction, int nFileLine, int nCode, char* pszMessage);
#define VERIFYC(x,c,y,z...) if (!(x)) { SetExceptionCode (c); VERIFY (x,y,##z); } else { errno = 0; }
#define VERIFY(x,y, z...) if (!(x)) { ExceptionHandle (__FILE__, __PRETTY_FUNCTION__, __LINE__, #x, y [0] == '\0' ? strerror (errno) : y, ##z); IfException (); throw this;  } else { errno = 0; }

//Exception by throwing a char* instead
#define CHECK(x,y) if (!(x)) { _EX_GENERATEGLOBALERROR (x,y); throw strGlobalStrError; }


#define _EX_GENERATEGLOBALERROR(x,y) strGlobalStrError.clear (); strGlobalStrError = strLastException  + __FILE__ + "(" + STRINGIZE(__LINE__) + ") Func: [" + __PRETTY_FUNCTION__ + "] Comm: [" + #x + "] Msg: ["; strGlobalStrError = strGlobalStrError + "" + (const char*)(y [0] == '\0' ? strerror (errno) : y) + "] ";

#define Verify(x,y,z) if (!(x)) { _EX_GENERATEGLOBALERROR (x,y);  YYTRACE ("%s", strGlobalStrError.c_str ()); fputc ('\n', stderr); return z; } else { errno = 0; }

#define Verify2(x,y) if (!(x)) { _EX_GENERATEGLOBALERROR (x,y); throw strGlobalStrError; }

#define Assert(x,y,z) if (!(x)) { TRACE ("%s(%u) Func: %s Comm: %s Msg: %s (Errno: [%u])\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, #x, y [0] == '\0' ? strerror (errno) : y, errno); exit (z); } else { errno = 0; }


#define ExceptionWhith(){\
		char* pszFile;\
		char* pszFunction;\
		int   nFileLine;\
		char* pszCode;\
		char* pszMessage;\
\
		pSocket->GetExceptionData (pszFile, pszFunction, &nFileLine, pszCode, pszMessage);\
		ExceptionHandle (pszFile, pszFunction, nFileLine, pszCode, pszMessage);\
	}

#define SAFENEW(NewAlloc, y, z...) try { NewAlloc; } catch (Exception& pException) { ExceptionHandle (__FILE__, __PRETTY_FUNCTION__, __LINE__, #NewAlloc, y [0] == '\0' ? strerror (errno) : y, ##z); IfException (); throw this; }
#define SafeNew(NewAlloc, y) try { NewAlloc; } catch (Exception& pException) { throw y [0] == '\0' ? strerror (errno) : y; }


class API_EXPORT Exception
{
private:
	long unsigned int nExceptionCode;
	const char*		pszFunction;
	const char*		pszFile;
	int				nFileLine;
	const char*		pszCode;	
	char            pszExceptionMessage [EXCEPTION_MESSAGE_SIZE + 1];
	char			pszMessage [EXCEPTION_MESSAGE2_SIZE + 1];

public:

    void SetExceptionCode (int nExceptionCode);
    int GetThrownExceptionCode ();

	void ExceptionHandle (const char* pszFile, const char* pszFunction, int nFileLine, const char* pszCode,  const char* pszFormat, ...);
	
	bool GetExceptionData (const char* pszFile, const char* pszFunction, int* nFileLine, const char* pszCode, const char* pszMessage);

	//bool  GetExceptionData (char* pszFile, char* pszFunction, int* nFileLine, char* pszCode, char* pszMessage);
	char* GetExceptionMessage ();
    void IfException ();
    
    API_EXPORT Exception () {  return; }
};

#endif
