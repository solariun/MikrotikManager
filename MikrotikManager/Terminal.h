/*
 *  Terminal.h
 *  Framework
 *
 *  Created by Gustavo Campos on 6/29/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
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

#ifndef TERMINAL_H
#define TERMINAL_H 1

#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <Util.h>
#include <Exception.h>
#include <vector>


#define TERM_MAX_TGRAGH 1024




class Terminal
{
private:
	bool Use256C;
	FILE* pFd;
	
	/* Funcoes para SOCKET REMOTO COM TTY/ANSI */
	int  TTY_STAT;
	struct  termios  pTerm_Temp;
	
	void StandardInitialization ();
public:

    class TGraph
    {

    private:
        float nMax;
        float nDataArray [TERM_MAX_TGRAGH + 1];
        bool  bScroll;

        uint32_t nOffset;

    public:
        TGraph ();
        ~TGraph ();

        TGraph& operator= (float nValue);

        void PlotGraphics (uint nX, uint nY, uint nWidth, uint nHeight, float nMax);
    };

    
	Terminal (FILE* pFd);
	Terminal ();

    void PrintOneBigNum (uint nX, uint nY, uint8_t nNum);
    void PrintBigNum (uint nX, uint nY, float nNum, bool bPercent);

	/* Terminal mode and color selection functions*/
	void Enable256Colors (bool nEnable);
	int  ReadColor(const char* rgb_string, unsigned char* output);
	void XTerm2RGB(unsigned char color, unsigned char* rgb);
	void MakeTable();
	unsigned char RGB2XTerm(unsigned char* rgb);
	bool GetTermSize (unsigned int *x, unsigned int *y);
	int DisableCanonical ();
	int ResetTerminal ();
	static int RGB (unsigned int R, unsigned int G, unsigned int B);
	uint GetColorByRGB (char* pszRGB);
	void SetColorByRGB (char* pszFGRGB, char* pszBGRGB);
	void SetColor (uint nFGColor, uint nBGColor);

	/* Direct Terminal output and input Functions*/ 
	void BigChar ();
	void EndScroll ();
	void Locate (int x,int y);
	void SetScroll (int x,int y);
	void Cls ();
	void ErrEndLine ();
	void ErrStartLine ();
	void ErrLine ();
	void ErrDown ();
	void ErrUp ();
	void PrintScr ();
	void PrintLine ();
	void StartPrint ();
	void StopPrint ();
	void EnableEcho ();
	void DisableEcho ();
	void Color (int at,int c1,int c2);
	void ColorInvert ();
	void ColorNormal ();
	void ColorUnderScore ();
	void ColorBlink ();
	void ColorBright ();
	void EnableGraphicChar (bool bSet);
	char GetKey ();

	void Box (int nX, int nY, int nX2, int nY2, char* pszTitle);
	void ClearBox (int nX, int nY, int nX2, int nY2);
	
	void RestoreCursorPosition ();
	void SaveCursorPosition ();
	void RestoreSecondaryMonitor ();
	void BeginSecondaryMonitor ();
	void ResetAttr ();
	
	bool Printf (const char* pszFormat, ...);
};

static Terminal Term;

#endif
