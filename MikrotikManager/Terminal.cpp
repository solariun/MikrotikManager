/*
 *  Terminal.cpp
 *  Framework
 *
 *  Created by Gustavo Campos on 6/29/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Terminal.h"

// whole colortable, filled by maketable()
static int initialized=0;
static unsigned char colortable[254][3];


// the 6 value iterations in the xterm color cube
static const unsigned char valuerange[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

// 16 basic colors
static const unsigned char basic16[16][3] =
{
	{ 0x00, 0x00, 0x00 }, // 0
	{ 0xCD, 0x00, 0x00 }, // 1
	{ 0x00, 0xCD, 0x00 }, // 2
	{ 0xCD, 0xCD, 0x00 }, // 3
	{ 0x00, 0x00, 0xEE }, // 4
	{ 0xCD, 0x00, 0xCD }, // 5
	{ 0x00, 0xCD, 0xCD }, // 6
	{ 0xE5, 0xE5, 0xE5 }, // 7
	{ 0x7F, 0x7F, 0x7F }, // 8
	{ 0xFF, 0x00, 0x00 }, // 9
	{ 0x00, 0xFF, 0x00 }, // 10
	{ 0xFF, 0xFF, 0x00 }, // 11
	{ 0x5C, 0x5C, 0xFF }, // 12
	{ 0xFF, 0x00, 0xFF }, // 13
	{ 0x00, 0xFF, 0xFF }, // 14
	{ 0xFF, 0xFF, 0xFF }  // 15
};


/*
 * Standard initialization 
 */
void Terminal::StandardInitialization ()
{
	Use256C = false;
	TTY_STAT = 0;
}



/*
 * Initiate the Terminal Object to use an given File Descriptor 
 */
Terminal::Terminal (FILE* pFd)
{
	Assert (pFd != NULL, "Error, a valid FILE object must be supplied.", -1);
	
	this->pFd = pFd;
	
	StandardInitialization ();
}

/* 
 * Init the terminal through the standard out
 */

Terminal::Terminal ()
{
	this->pFd = stdout;
	StandardInitialization ();
}


void Terminal::Enable256Colors (bool nEnable)
{
	this->Use256C = nEnable;
}





// read a hex-rgb-color like "CCAABB"
// output are 3 unsigned chars
// returns 0 on failure and 1 on success
int Terminal::ReadColor(const char* rgb_string, unsigned char* output)
{
	char Xr[3], Xg[3], Xb[3];
	
	// string -> bytes
	if(strlen(rgb_string)!=6) return 0;
	strncpy(Xr,rgb_string+0,2);
	strncpy(Xg,rgb_string+2,2);
	strncpy(Xb,rgb_string+4,2);
	output[0] = (unsigned char) strtoll(Xr, NULL, 16);
	output[1] = (unsigned char) strtoll(Xg, NULL, 16);
	output[2] = (unsigned char) strtoll(Xb, NULL, 16);
	
	return 1;
}

// convert an xterm color value (0-253) to 3 unsigned chars rgb
void Terminal::XTerm2RGB(unsigned char color, unsigned char* rgb)
{
	// 16 basic colors
	if(color<16)
	{
		rgb[0] = basic16[color][0];
		rgb[1] = basic16[color][1];
		rgb[2] = basic16[color][2];
	}
	
	// color cube color
	if(color>=16 && color<=232)
	{
		color-=16;
		rgb[0] = valuerange[(color/36)%6];
		rgb[1] = valuerange[(color/6)%6];
		rgb[2] = valuerange[color%6];
	}
	
	// gray tone
	if(color>=233 && color<=253)
	{
		rgb[0]=rgb[1]=rgb[2] = 8+(color-232)*0x0a;
	}
}

// fill the colortable for use with rgb2xterm
void Terminal::MakeTable()
{
	unsigned char c, rgb[3];
	for(c=0;c<=253;c++)
	{
		XTerm2RGB(c,rgb);
		colortable[c][0] = rgb[0];
		colortable[c][1] = rgb[1];
		colortable[c][2] = rgb[2];
	}
}

// selects the nearest xterm color for a 3xBYTE rgb value
unsigned char Terminal::RGB2XTerm(unsigned char* rgb)
{
	unsigned char c, best_match=0;
	double d, smallest_distance;
	
	if(!initialized)
		MakeTable();
	
	smallest_distance = 10000000000.0;
	
	for(c=0;c<=253;c++)
	{
		d = pow(colortable[c][0]-rgb[0],2.0) + 
		pow(colortable[c][1]-rgb[1],2.0) + 
		pow(colortable[c][2]-rgb[2],2.0);
		if(d<smallest_distance)
		{
			smallest_distance = d;
			best_match=c;
		}
	}
	
	return best_match;
}


/* nquire actual terminal size (this it what the
 * kernel thinks - not was the user on the over end
 * of the phone line has really).
 */
bool  Terminal::GetTermSize (unsigned int *x, unsigned int *y)
{
	int fd = fileno (this->pFd);
	
#ifdef TIOCGSIZE
    struct ttysize win;
    
#elif defined(TIOCGWINSZ)
    struct winsize win;
    
#endif
    Verify (x != NULL, "O Ponteiro para X fornecido esta nulo.", false);
	Verify (y != NULL, "O Ponteiro para Y fornecido esta nulo.", false);
	Verify (fd >= 0, "O File descriptor informado está invalido.", false);
#ifdef TIOCGSIZE
    if (ioctl (fd, TIOCGSIZE, &win))
    {
        return false;
    }
    
    if (y)
    {
        *y=win.ts_lines;
    }
    
    if (x)
    {
        *x=win.ts_cols;
    }
    
#elif defined TIOCGWINSZ
    if (ioctl (fd, TIOCGWINSZ, &win))
    {
        return false;
    }
    
    if (y)
    {
        *y=win.ws_row;
    }
    
    if (x)
    {
        *x=win.ws_col;
    }
    
#else
    {
        const char *s;
		
        s=getenv("LINES");
        
        if (s)
        {
            *y=strtol(s,NULL,10);
        }
        else
        {
            *y=25;
        }
        
        s=getenv("COLUMNS");
        
        if (s)
        {
            *x=strtol(s,NULL,10);
        }
        else
        {
            *x=80;
        }
    }
#endif
	
    return true;
}



/*
 * Desabilita o modo canonical para 
 * terminais.
 *  - O modo canonico especifica um buffer
 * para o TTY que é disponibilizado no fd
 * após se precionar <enter>
 */


int Terminal::DisableCanonical ()
{
	int nFd = fileno (this->pFd);
	
	struct termios Term;
	
	Verify (nFd >= 0, "O File descriptor informado está invalido.", false);
	
	if (tcgetattr (nFd, &pTerm_Temp) < 0)
	{
		fprintf (this->pFd, "%s\n", strerror (errno));
		return (-1);
	}
	
	Term = pTerm_Temp;
	
	Term.c_lflag &= ~(ECHO | ICANON); /* No Echo, Chanonical off */
	
	Term.c_cc [VMIN]   = 1;  /* No timer */
	Term.c_cc [VTIME]  = 0;
	
	if (tcsetattr (nFd, TCSAFLUSH, &Term) < 0)
	{
		return -1;
	}
	
	TTY_STAT = 1;
	
	return 1;
}

/* 
 * Retorna o TTY ao antigo estado, deve-se ressaltar 
 * que essa opcao deve ser usada somente quando
 * o modo canonico já fora desabilitando usando a função 
 * TTY_DisableCanonical.
 */

int Terminal::ResetTerminal ()
{
	int nFd = fileno (this->pFd);

	Verify (nFd >= 0, "O File descriptor informado está invalido.", false);
	
	if (TTY_STAT != 1)
	{
		return 1;
	}
	
	if (tcsetattr (nFd, TCSAFLUSH, &pTerm_Temp) < 0)
	{
		return -1;
	}
	
	return 0;
}

int Terminal::RGB (unsigned int R, unsigned int G, unsigned int B)
{
	return (16+(R*36)+(G*6)+B);
}

/* Imprime caracteres 2x */
void Terminal::BigChar ()
{
    fprintf (this->pFd, "\033#6");
}

/* Finalizar Scroll de tela caso exista */
void Terminal::EndScroll ()
{ 
    fprintf (this->pFd, "\033[r");
}


/* Coloca o cursor do terminal em um local especifico. */
void Terminal::Locate (int x,int y)
{
    fprintf (this->pFd, "\033[%d;%df",x,y);
}

/* Inicia o scroll do terminal na linha X a linha Y */
void Terminal::SetScroll (int x,int y)
{
    fprintf (this->pFd, "\033[?6h \033[%.2d;%.2dr",x,y);
}


/* Limpa a tela do terminal */
void Terminal::Cls ()
{
    fprintf (this->pFd, "\033[2J");
}


/* Limpa do local ate o fim da linha */
void Terminal::ErrEndLine ()
{ 
    fprintf (this->pFd,"\033[K");
}

/* Limpar do local ate o comesso da linha */
void Terminal::ErrStartLine ()
{ 
    fprintf (this->pFd, "\033[1K");
}

/* Limpar a linha toda */
void Terminal::ErrLine ()
{
    fprintf (this->pFd, "\033[2K");
}

/* limpar para baixo */
void Terminal::ErrDown ()
{ 
    fprintf (this->pFd, "\033[J");
}


/* Limpar para cima */
void  Terminal::ErrUp ()
{ 
   fprintf (this->pFd, "\033[1J");
}


/* impressora remota e local */
void Terminal::PrintScr ()
{ 
    fprintf (this->pFd, "\033[i");
}


/* Imprime a linha corrente */
void Terminal::PrintLine ()
{ 
    fprintf (this->pFd, "\033[1i");
}


/* Desvia todos os dados para a impressora */
void Terminal::StartPrint ()
{ 
    fprintf (this->pFd, "\033[5i");
}


/* Para o desvio para a impressora */
void Terminal::StopPrint ()
{ 
    fprintf (this->pFd, "\033[4i");
}


/* Atributos de telas */
void Terminal::EnableEcho ()
{
    printf ( "\033[8h");
}


/* Desabilita Echo local por terminal */
void Terminal::DisableEcho ()
{
    printf ( "\033[8l");
}

/* Atributos para 16 CORES ONLY.
       At (Atributos):
		0-Normal, 
		1-Brilhant, 
		5-Piscando, 
		7 -Invertido,
		4-Sublinhado 
 */

void Terminal::Color ( int at,int c1,int c2)
{ 
    fprintf (this->pFd, "\033[%d;%d;%dm",at, (c1+30), (c2+40));
}


/* Inverter cores correntes */
void Terminal::ColorInvert ()
{ 
    fprintf (this->pFd, "\033[7m");
}


/* volta a cor para o estado normal */
void Terminal::ColorNormal ()
{
    fprintf (this->pFd, "\033[0m");
}


/* Coloca um sublinhado em Terminal:: */
void Terminal::ColorUnderScore ()
{ 
    fprintf (this->pFd, "\033[4m");
}


/* Colocar a cor piscando */
void Terminal::ColorBlink ()
{ 
    fprintf (this->pFd, "\033[5m");
}


void Terminal::ColorBright ()
{ /* cor mais brilhante */
    fprintf (this->pFd, "\033[1m");
}


void Terminal::EnableGraphicChar (bool bEnable)
{
	if (bEnable) 
	{
		fprintf (this->pFd, "\033(0");
	}
	else 
	{
		fprintf (this->pFd, "\033(B");
	}
	
}


/* Recupera um caracter do stdin,
 * para ser usado com eficiencia, deve ser usado
 * com o modo canonico desligado
 */
char Terminal::GetKey ()
{
	char chKey = 0;
	
	fread (&chKey, 1, 0, pFd);
	
	return chKey;
}



/*
 * Desenha um box na tela, podendo usar inclusive um título
 */
void Terminal::Box (int nX, int nY, int nX2, int nY2, char* pszTitle)
{
	int nCount;
	int nCount2;
	
	fprintf (this->pFd, "\033(0");
	
	Locate (nX, nY);
	
	fprintf (this->pFd, "l"); 
	
	for (nCount = (nY + 1); nCount < (nY2-1); nCount++)
	{
		fprintf (this->pFd, "q");
	}
	
	fprintf (this->pFd, "k");
	
	for (nCount2 = (nX + 1); nCount2 < (nX2); nCount2++)
	{
		Locate (nCount2, nY);
	    fprintf (this->pFd, "x%*sx", (nY2 - nY - 2), " ");
	}
	
	Locate (nX2, nY);
	
	fprintf (this->pFd, "m");
	
	for (nCount = (nY + 1); nCount < (nY2-1); nCount++)
	{
		fprintf (this->pFd, "q");
	}
	
	fprintf (this->pFd, "j");
	
	
	if (pszTitle != NULL)
	{
		Locate (nX, nY + 2);
		fprintf (this->pFd, "%c\033(B%s\033(0%c", 117, pszTitle, 116);
	}

	fprintf (this->pFd, "\033(B");

	fflush (this->pFd);
}


/* 
 * Imprime uma área com espacos nas coordenadas especificadas.
 */

void Terminal::ClearBox (int nX, int nY, int nX2, int nY2)
{
	int nCount = 0;
	
	for (nCount=nX; nCount <= nX2; nCount++)
	{
		Locate (nCount, nY);
		fprintf (this->pFd, "%*s", nY - nY2, " ");
	}
}




/* Reset terminal attributes */
void Terminal::ResetAttr ()
{
	fprintf (this->pFd, "\033[0m");
    fprintf (this->pFd, "\033(B");
}


/* Inicializa monitor secundário */
void Terminal::BeginSecondaryMonitor ()
{
	fprintf (this->pFd, "\033%c\033[?47h", '7');
}


/* Restaura monitor */
void Terminal::RestoreSecondaryMonitor ()
{
	fprintf (this->pFd, "\033[?47l\033%c", '8');
}


/* Save Cursor */
void Terminal::SaveCursorPosition ()
{
	fprintf (this->pFd, "\033%c", '7');
}


/* Restaura Cursor */
void Terminal::RestoreCursorPosition ()
{
	fprintf (this->pFd, "\033%c", '8');
}



void Terminal::SetColor (uint nFGColor, uint nBGColor)
{		
	if (Use256C == true)
	{
		fprintf (this->pFd, "\033[38;5;%um", nFGColor);
		fprintf (this->pFd, "\033[48;5;%um", nBGColor);
	}
	else
	{		
		Color (0, nFGColor, nBGColor);
	}
}


void Terminal::SetColorByRGB (char* pszFGRGB, char* pszBGRGB)
{
	uint nColor;
	uint nBGColor;
	
	
	if (Use256C == true)
	{
		if (pszFGRGB != NULL)
		{
			nColor = RGB2XTerm ((unsigned char*) pszFGRGB);
			fprintf (this->pFd, "\033[38;5;%um", nColor);
		}
		
		if (pszBGRGB != NULL)
		{
			nColor = RGB2XTerm ((unsigned char*) pszBGRGB);
			fprintf (this->pFd, "\033[48;5;%um", nColor);
		}
	}
	else
	{
		nColor = RGB2XTerm ((unsigned char*)pszFGRGB) % 16;
		nBGColor = RGB2XTerm ((unsigned char*)pszBGRGB) % 16;
		
		Color (0, nColor, nBGColor);
	}
}


uint Terminal::GetColorByRGB (char* pszRGB)
{
	if (pszRGB == NULL)
	{
		return 0;
	}
	
	if (Use256C == true)
	{
		return RGB2XTerm ((unsigned char*)pszRGB);
	}
	else
	{
		return (RGB2XTerm ((unsigned char*)pszRGB) % 16);
	}
	
	return 0;
}


bool Terminal::Printf (const char* pszFormat, ...)
{
	va_list   vaArgs;
	
	//signal (SIGPIPE, Socket::PipeReceive);
	
	va_start (vaArgs, pszFormat);
	
	Verify (vfprintf (pFd, pszFormat, vaArgs) _ERROR, "Erro ao processar a string a ser enviada.", false);
		
	va_end (vaArgs);
		
	return true;
}


void Terminal::PrintOneBigNum (uint nX, uint nY, uint8_t nNum)
{
 //   const char szNumbers[] = " XXX XX XXXX XXXX XX XXX ; XXX   XX   XX   XX XXXXX;XXXX    XX XXX XX   XXXXX;XXXX    XX XXX    XXXXXX ;XX XXXX XXXXXXX   XX   XX;XXXXXXX   XXXX    XXXXXX ; XXXXXX   XXXX XX XX XXX ;XXXXX   XX  XX  XX  XX   ; XXX XX XX XXX XX XX XXX ; XXX XX XX XXXX   XX XXX ;";

    const char szNumbers[] = " aaa aa aaaa aaaa aa aaa ; aaa   aa   aa   aa aaaaa;aaaa    aa aaa aa   aaaaa;aaaa    aa aaa    aaaaaa ;aa aaaa aaaaaaa   aa   aa;aaaaaaa   aaaa    aaaaaa ; aaaaaa   aaaa aa aa aaa ;aaaaa   aa  aa  aa  aa   ; aaa aa aa aaa aa aa aaa ; aaa aa aa aaaa   aa aaa ;     a  aa  aa  aa  aa  a;";


    uint nLocation = (nNum * 26);

    fprintf (this->pFd, "\033(0");
    for (uint nCount = 0; nCount < 5; nCount++)
    {
        Locate (nX + nCount, nY);
        fprintf (this->pFd, "%.5s", &szNumbers [nLocation]);
        nLocation += 5;
    }
    fprintf (this->pFd, "\033(5");
}


void Terminal::PrintBigNum (uint nX, uint nY, float nNum, bool bPercent)
{
    char szNumber [30];

    if (nNum == (int) nNum)
    {
        if (bPercent == false)
            snprintf (szNumber, sizeof (szNumber) - 1, "%u", (uint)nNum);
        else
            snprintf (szNumber, sizeof (szNumber) - 1, "%u:", (uint)nNum);
    }
    else
    {
        if (bPercent == false)
            snprintf (szNumber, sizeof (szNumber) - 1, "%.1f", nNum);
        else
            snprintf (szNumber, sizeof (szNumber) - 1, "%.1f:", nNum);
    }

    uint32_t nLen = (uint32_t) strlen (szNumber);
    uint32_t nLoc = nY;

    fprintf (this->pFd, "\033(B");

    for (uint nCount=0; nCount < nLen; nCount++)
    {
        if (szNumber [nCount] == '.')
        {
            Locate (nX+4, nLoc);
            fprintf (this->pFd, "XX");

            nLoc += 3;
        }
        else
        {
            PrintOneBigNum (nX, nLoc, (uint8_t) szNumber [nCount] - '0');
            nLoc += 6;
        }
    }

}




/* PLOT STATISTICAL GRAPHS

    Limitations:
        Works only with positive data.

*/

Terminal::TGraph::TGraph ()
{
    nOffset = 0;
    bScroll = 0;
    nMax = 0;
}

Terminal::TGraph::~TGraph ()
{
    return;
}

Terminal::TGraph& Terminal::TGraph::operator= (float nValue)
{
    nDataArray [nOffset++] = nValue;


    if (nMax <= nValue) nMax = nValue;

    if (nOffset > TERM_MAX_TGRAGH)
    {
        bScroll = true;
        nOffset = 0;
    }

    return *this;
}


void Terminal::TGraph::PlotGraphics (uint nX, uint nY, uint nWidth, uint nHeight, float nMax)
{
    uint nYY = nY + nWidth;
    uint nXX = nX + nHeight;

    Term.Color (0, 0, 7);

    Term.Box (nX, nY, nXX, nYY, NULL);

    uint32_t nOffset;
    uint32_t nPossition, nPorcentage;
    uint32_t nGraphOffset = 0;

    if (this->nOffset == 0 && bScroll == false)
    {
        return;
    }
    else if (this->nOffset == 0 && bScroll == true)
    {
        nOffset = TERM_MAX_TGRAGH;
    }
    else
    {
        nOffset = this->nOffset - 1;
    }

    uint32_t nPData [nWidth - 2];
    memset ((void*) &nPData, 0 , sizeof (nPData));

    for (uint32_t nCount=0; nCount < TERM_MAX_TGRAGH; nCount++)
    {
        if (nCount > (nWidth - 2)) break;

        nPData [nCount] = (uint32_t) ((float) (nDataArray [nOffset] * 100) / nMax);
        
        //nPData [nCount] = ((nHeight - 2) * nPData [nCount]) / 100;

        //TRACE ("nPData [nCount (%u)] = %u\n", nCount, nPData [nCount]);

        if (nOffset == 0 && bScroll == true)
        {
            nOffset = TERM_MAX_TGRAGH;
        }
        else if (nOffset == 0 && bScroll == false)
        {
            break;
        }
        else
        {
            nOffset--;
        }
    }


    nGraphOffset = 0;

    uint32_t nValue [2];

    for (uint32_t nCountX=nX+1; nCountX < nXX; nCountX++)
    {
        for (uint32_t nCount=0; nYY - nCount > nY+2; nCount++)
        {
            if (nPData [nCount] > 70)
            {
                Term.Color (1, 1, 1);
            }
            else if (nPData [nCount] > 50)
            {
                Term.Color (1, 3, 3);
            }
            else
            {
                Term.Color (1, 4, 4);
            }

            nValue [0] = nPData [nCount];
            nValue [0] = (((nHeight - 1) * nPData [nCount]) / 100);
            nValue [1] = (nXX - nCountX);

            if (nValue [0] >= nValue [1] /*&&  nValue [0] <= nValue [1]*/)
            {
                Term.Locate (nCountX, nYY - nCount - 2); printf (" ");
            }
            else if (nValue [0] == 0 && nCountX == nXX - 1)
            {
                Term.Color (0, 4, 7);
                Term.Locate (nCountX, nYY - nCount - 2); printf ("_");
            }
        }
    }
}
