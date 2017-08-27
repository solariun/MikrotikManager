/*
 *  Exception.cpp
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


