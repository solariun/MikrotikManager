/*
 *  Exception.cpp
 *  thread
 *
 *  Created by Gustavo Campos on 24/04/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <Exception.h>
#include <Util.h>


void Exception::SetExceptionCode (int nExceptionCode)
{
    this->nExceptionCode = nExceptionCode;
}

int Exception::GetThrownExceptionCode ()
{
    return nExceptionCode;
}


char* Exception::GetExceptionMessage ()
{
	snprintf (pszExceptionMessage, EXCEPTION_MESSAGE_SIZE, "%s(%u) Func: [%s] Comm: [%s] Msg: %s", this->pszFile, this->nFileLine, this->pszFunction, this->pszCode, this->pszMessage);
	
	return pszExceptionMessage;
}


void Exception::IfException ()
{
    TRACE ("Default Exception treatement risen.\n");
}


bool Exception::GetExceptionData (const char* pszFile, const char* pszFunction, int* nFileLine, const char* pszCode, const char* pszMessage)
{
	
	if (pszFile != NULL) pszFile = this->pszFile;
	if (pszFunction != NULL) pszFunction = this->pszFunction;
	if (pszCode != NULL) pszCode = this->pszCode;
	if (pszMessage != NULL) pszMessage = (const char*) &this->pszMessage;
	if (nFileLine != NULL) *nFileLine = this->nFileLine;
	
	return true;
}


void Exception::ExceptionHandle (const char* pszFile, const char* pszFunction, int nFileLine, const char* pszCode,  const char* pszFormat, ...)
{
		va_list   vaArgs;
        long int nLen;
                        
        va_start (vaArgs, pszFormat);
        
        VERIFY ((nLen = vsnprintf ((char*) this->pszMessage, EXCEPTION_MESSAGE2_SIZE, pszFormat, vaArgs)) _ERROR, "Erro ao processar a string a ser enviada.");
        
        this->pszMessage [nLen] ='\0';
        
        va_end (vaArgs);

	  this->pszFile				= pszFile;
	  this->pszFunction			= pszFunction;
	  this->nFileLine           = nFileLine;
	  this->pszCode             = pszCode;
	  
	  //YYTRACE  ("this->Msg: [%s]\n\n", this->pszMessage);
}


