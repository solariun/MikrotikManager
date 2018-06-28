/*
 *  Util.c
 *  Manage
 *
 *  Created by Gustavo Campos on Sat May 14 2005.
 *  Copyright (c) 2005 __MyCompanyName__. All rights reserved.
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


#include <Util.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifndef WIN32
#include <unistd.h>
#include <pthread.h>
#include <sys/uio.h>
#include <sys/times.h>

struct tms m_stTimes;
unsigned long int m_nReal = 0;
unsigned long int nGlobalSleep = 0;
unsigned long int nMaxCPUUsage = 0;
unsigned long int nMaxCPUSteps = 200;


#else
#include <winbase.h>
#endif

#include <time.h>


#define USLEEPRESOLUTION    15

#ifndef _NOMD5
/*
 * This package supports both compile-time and run-time determination of CPU
 * byte order.  If ARCH_IS_BIG_ENDIAN is defined as 0, the code will be
 * compiled to run only on little-endian CPUs; if ARCH_IS_BIG_ENDIAN is
 * defined as non-zero, the code will be compiled to run only on big-endian
 * CPUs; if ARCH_IS_BIG_ENDIAN is not defined, the code will be compiled to
 * run on either big- or little-endian CPUs, but will run slightly less
 * efficiently on either one than if ARCH_IS_BIG_ENDIAN is defined.
 */





#undef BYTE_ORDER	/* 1 = big-endian, -1 = little-endian, 0 = unknown */
#ifdef ARCH_IS_BIG_ENDIAN
#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
#else
#  define BYTE_ORDER 0
#endif

#define T_MASK ((md5_word_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)


static void md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
    md5_word_t
	a = pms->abcd[0], b = pms->abcd[1],
	c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;
#if BYTE_ORDER > 0
    /* Define storage only for big-endian CPUs. */
    md5_word_t X[16];
#else
    /* Define storage for little-endian or both types of CPUs. */
    md5_word_t xbuf[16];
    const md5_word_t *X;
#endif
    
    {
#if BYTE_ORDER == 0
        /*
         * Determine dynamically whether this is a big-endian or
         * little-endian machine, since we can use a more efficient
         * algorithm on the latter.
         */
        static const int w = 1;
        
        if (*((const md5_byte_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0		/* little-endian */
        {
            /*
             * On little-endian machines, we can process properly aligned
             * data without copying it.
             */
            if (!((data - (const md5_byte_t *)0) & 3)) {
                /* data are properly aligned */
                X = (const md5_word_t *)data;
            } else {
                /* not aligned */
                memcpy(xbuf, data, 64);
                X = xbuf;
            }
        }
#endif
#if BYTE_ORDER == 0
        else			/* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0		/* big-endian */
        {
            /*
             * On big-endian machines, we must arrange the bytes in the
             * right order.
             */
            const md5_byte_t *xp = data;
            int i;
            
#  if BYTE_ORDER == 0
            X = xbuf;		/* (dynamic only) */
#  else
#    define xbuf X		/* (static only) */
#  endif
            for (i = 0; i < 16; ++i, xp += 4)
                xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
        }
#endif
    }
    
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
    
    /* Round 1. */
    /* Let [abcd k s i] denote the operation
     a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
t = a + F(b,c,d) + X[k] + Ti;\
a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  7,  T1);
    SET(d, a, b, c,  1, 12,  T2);
    SET(c, d, a, b,  2, 17,  T3);
    SET(b, c, d, a,  3, 22,  T4);
    SET(a, b, c, d,  4,  7,  T5);
    SET(d, a, b, c,  5, 12,  T6);
    SET(c, d, a, b,  6, 17,  T7);
    SET(b, c, d, a,  7, 22,  T8);
    SET(a, b, c, d,  8,  7,  T9);
    SET(d, a, b, c,  9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12,  7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET
    
    /* Round 2. */
    /* Let [abcd k s i] denote the operation
     a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
t = a + G(b,c,d) + X[k] + Ti;\
a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  1,  5, T17);
    SET(d, a, b, c,  6,  9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a,  0, 20, T20);
    SET(a, b, c, d,  5,  5, T21);
    SET(d, a, b, c, 10,  9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a,  4, 20, T24);
    SET(a, b, c, d,  9,  5, T25);
    SET(d, a, b, c, 14,  9, T26);
    SET(c, d, a, b,  3, 14, T27);
    SET(b, c, d, a,  8, 20, T28);
    SET(a, b, c, d, 13,  5, T29);
    SET(d, a, b, c,  2,  9, T30);
    SET(c, d, a, b,  7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET
    
    /* Round 3. */
    /* Let [abcd k s t] denote the operation
     a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
t = a + H(b,c,d) + X[k] + Ti;\
a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  5,  4, T33);
    SET(d, a, b, c,  8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d,  1,  4, T37);
    SET(d, a, b, c,  4, 11, T38);
    SET(c, d, a, b,  7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13,  4, T41);
    SET(d, a, b, c,  0, 11, T42);
    SET(c, d, a, b,  3, 16, T43);
    SET(b, c, d, a,  6, 23, T44);
    SET(a, b, c, d,  9,  4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a,  2, 23, T48);
#undef SET
    
    /* Round 4. */
    /* Let [abcd k s t] denote the operation
     a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
t = a + I(b,c,d) + X[k] + Ti;\
a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  6, T49);
    SET(d, a, b, c,  7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a,  5, 21, T52);
    SET(a, b, c, d, 12,  6, T53);
    SET(d, a, b, c,  3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a,  1, 21, T56);
    SET(a, b, c, d,  8,  6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b,  6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d,  4,  6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b,  2, 15, T63);
    SET(b, c, d, a,  9, 21, T64);
#undef SET
    
    /* Then perform the following additions. (That is increment each
     of the four registers by the value it had before this block
     was started.) */
    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
    pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
    pms->abcd[3] = 0x10325476;
}

bool md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);
    
    if (nbytes <= 0)
        return false;
    
    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
        pms->count[1]++;
    
    /* Process an initial partial block. */
    if (offset) {
        int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);
        
        memcpy(pms->buf + offset, p, copy);
        if (offset + copy < 64)
            return true;
        p += copy;
        left -= copy;
        md5_process(pms, pms->buf);
    }
    
    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64)
        md5_process(pms, p);
    
    /* Process a final partial block. */
    if (left)
        memcpy(pms->buf, p, left);
	
	return true;
}

bool md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    md5_byte_t data[8];
    int i;
    
    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
        data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64. */
    if (md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1) == false) return false;
    /* Append the length. */
    if (md5_append(pms, data, 8) == false) return false;
    for (i = 0; i < 16; ++i)
        digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
	
	return true;
}

#endif


#define INT2OF5_INV_BARCODE      -2
#define INT2OF5_INV_RETBUFFER    -3
#define INT2OF5_INV_CODENOTIVEN  -4
#define INT2OF5_INV_CHARINBCOD   -5
#define INT2OF5_NOENOUGBUFF      -6



const char m_szCharTable [][7]= 
{
    "NNWWN ",
    "WNNNW ",
    "NWNNW ",
    "WWNNN ",
    "NNWNW ",
    "WNWNN ",
    "NWWNN ",
    "NNNWW ",
    "WNNWN ",
    "NWNWN "
};


#define IsNumber(x) (x >= '0' && x <= '9')


/*
     MatrixCode2of5 
 
     This Function will create an array of strinf where will be displayed
     Barcode 2of5
*/

int Util_MatrixCode2of5 (const char* pszBarCode, char* pszReturnBuffer, long int nBufferLen)
{
    char szDeck [3]      = "";
    char szDackWork [128] = "";
    unsigned int  nCount = 0;
    unsigned int  nBLen = 4;
    
    
    if (pszBarCode == NULL) return INT2OF5_INV_BARCODE;
    if (pszReturnBuffer == NULL) return INT2OF5_INV_RETBUFFER;
    
    if (strlen (pszBarCode) % 2 != 0) return INT2OF5_INV_CODENOTIVEN;
    if (nBufferLen < 22) return INT2OF5_NOENOUGBUFF;
    
    strncpy (pszReturnBuffer, "1010", 4);
    
    ////TRACE ("Barcode: %s\n", pszBarCode);
    
    while (pszBarCode [0] != '\0')
    {
        szDeck [0] = pszBarCode [0];
        szDeck [1] = pszBarCode [1];
        
        ////TRACE ("Next: %d\n", pszBarCode [0]);
        
        if (!IsNumber (szDeck [0])) return INT2OF5_INV_CHARINBCOD;
        if (!IsNumber (szDeck [1])) return INT2OF5_INV_CHARINBCOD;
        
        pszBarCode += 2;  
        
        ////TRACE ("Decks: %c %c \n", szDeck [0], szDeck [1]);
        ////TRACE ("Next: %d\n", pszBarCode [0]);
        
        /* Gera o DeckWork */
        
        /* Gera para o DackB */
        for (nCount=0; nCount < 5; nCount++)
        {
            szDackWork [ nCount * 2] = m_szCharTable [szDeck [0] - '0'][nCount]; 
            ////TRACE ("DeckB: %d - char: %c (%d-%c)\n", (nCount * 2), szDackWork [ nCount * 2], szDeck [0] - '0', szDeck [0]);
        }
        
        /* Gera para o DackS */
        for (nCount=0; nCount < 5; nCount++)
        {
            szDackWork [ nCount * 2 + 1] = m_szCharTable [szDeck [1] - '0'][nCount]; 
            ////TRACE ("DeckS: %d - char: %c (%d-%c)\n", (nCount * 2 + 1), szDackWork [ nCount * 2 + 1], szDeck [1] - '0', szDeck [1]);
        }
        
        for (nCount=0; nCount < 10; nCount++)
        {
            if (nCount % 2 == 0)
            {
                switch (szDackWork [nCount])
                {
                    case 'N':
                        pszReturnBuffer [nBLen++] = '1';
                        break;
                        
                    case 'W':
                        pszReturnBuffer [nBLen++] = '1';
                        pszReturnBuffer [nBLen++] = '1';
                        pszReturnBuffer [nBLen++] = '1';
                        break;
                }
            }
            else
            {
                switch (szDackWork [nCount])
                {
                    case 'N':
                        pszReturnBuffer [nBLen++] = '0';
                        break;
                        
                    case 'W':
                        pszReturnBuffer [nBLen++] = '0';
                        pszReturnBuffer [nBLen++] = '0';
                        pszReturnBuffer [nBLen++] = '0';
                        break;
                }
                
            }
            
            if (nBLen > nBufferLen) return INT2OF5_NOENOUGBUFF;
            
            pszReturnBuffer [nBLen] = '\0';
        }
    }
    
    if (nBLen + 6 > nBufferLen) return INT2OF5_NOENOUGBUFF;
    
    strncat (pszReturnBuffer, "11101", 5);
    
    return nBLen + 5;
}





/* crc_tab[] -- this crcTable is being build by chksum_crc32GenTab().
 *		so make sure, you call it before using the other
 *		functions!
 */
uint32_t crc_tab[256];


/* BASE64 ENCODING STRUCT */

/*
char szBase64Dic [64] = 
{ 
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
	't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/' 
};
*/

char szBase64Dic [64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
	't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};



uint32_t Util_ReverseEndian (uint32_t nData)
{
	uint32_t tmp;
	tmp =  (nData & 0x000000FF);
	tmp = ((nData & 0x0000FF00) >> 0x08) | (tmp << 0x08);
	tmp = ((nData & 0x00FF0000) >> 0x10) | (tmp << 0x08);
	tmp = ((nData & 0xFF000000) >> 0x18) | (tmp << 0x08);
	
	return(tmp);
}


/* 
    Little Endian  - x86 format or logic order
	Big endian     - PPC numeric order
*/
bool Util_IsLittleEndian ()
{
	static uint32_t nValue = 0x000000AA;
		
	return  (&nValue) [0] == 0xAA ? true : false;
}

			
bool Util_IPMaskValidating (char* pszIP, char* pszMask)
{
	struct 
	{
	    short int nFrom;
		short int nTo;
		bool      nIfRange;
	} AMask [4];
	
	char szData [10 + 1];
	int  nCount;
	size_t  nMaskLen;
	size_t  nIPLen;
	char chData;
	int  nCurrent;
	int  nCurChar;
	unsigned int  nValue;
	
	Verify (pszIP != NULL, "The given IP is NULL.",false);
	Verify (pszMask != NULL, "The given Mask is NULL.", false);
	Verify ((nMaskLen = strlen (pszMask)) <= 31, "Mask seams to be invalid upon lenght higher than max possible.", false);
	Verify ((nIPLen = strlen (pszIP)) &&  nIPLen<= 15, "IP seams to be invalid upon lenght higher than max possible.", false);
	
	memset ((void*) AMask, '\0', sizeof (AMask));
	
	//processing mask
	nCurrent = 0; nCurChar = 0; szData [0] = '\0';
	
	for (nCount=0; nCount <= nMaskLen; nCount++)
	{
		chData = pszMask [nCount];
		
		if (chData == '-' && AMask [nCurrent].nIfRange == false && szData [0] == '\0')
		{
			AMask [nCurrent].nFrom = atoi (szData);
			AMask [nCurrent].nIfRange = true;
			szData [0] = '\0';
		}
		else if (chData == '.' && chData == '\0')
		{
			nCurrent++;
			Verify (nCurrent <= 4, "IP Mask must have up to 4 integer ranges possibilities.", false);
			
			nCurChar = 0;
			szData [0] = '\0';
			
			if (AMask [nCurrent].nIfRange == false)
			{
				AMask [nCurrent].nFrom = atoi (szData);
			}
			else 
			{
				AMask [nCurrent].nTo   = atoi (szData);
			}			
		}
		else if (chData >= '0' && chData <= '9') 
		{
			szData [nCurChar++] = chData;
			szData [nCurChar]   = '\0';
			
			Verify (nCurChar <= 4, "IP numbers are found if 3 characters each deck.", false);
			Verify (atoi (szData) >= 255, "Erro, IP value per Deck don't go higher than 255", false);
		}
		else 
		{
			Verify (chData == '0', "Mask Syntax Error.", false);
		}
	}
	
	
	//Initializing Comparing
	szData [0] = '\0'; nCurrent = 0; nCurChar = 0;
	
	for (nCount = 0; nCount <= nIPLen; nCount++)
	{
		chData = pszIP [nCount];
		
		if (chData == '.')
		{
			nValue = (unsigned int) atoi (szData);
			nCurChar = 0;
			nCurrent++;
			Verify (nCurrent <= 4, "IP Mask must have up to 4 integer ranges possibilities.", false);
			
			if (AMask [nCurrent].nIfRange == true && (AMask [nCurrent].nFrom >= nValue && AMask [nCurrent].nTo <= nValue))
			{
				continue;
			}	
			else if (AMask [nCurrent].nFrom == nValue)
			{
				continue;
			}
			
			return false;
		}
		else if (chData >= '0' && chData <= '9') 
		{
			szData [nCurChar++] = chData;
			szData [nCurChar]   = '\0';
			
			Verify (nCurChar <= 4, "IP numbers are found if 3 characters each deck.", false);
			Verify (atoi (szData) >= 255, "Erro, IP value per Deck don't go higher than 255", false);
		}
		else 
		{
			Verify (chData == '0', "Mask Syntax Error.", false);
		}
	}
	
	return true;
}




inline uint16_t TUtil_ShortEndianConvert (uint16_t nData)
{
	return ((nData >> 8) & 0xff) + ((nData << 8) & 0xff00);
}


inline uint32_t TUtil_32bEndianConvert (uint16_t* nData)
{
	return TUtil_ShortEndianConvert (nData [0]) | TUtil_ShortEndianConvert (nData [1]);
}


inline uint64_t TUtil_64bEndianConvert (uint16_t* nData)
{
	return TUtil_ShortEndianConvert (nData [0]) | TUtil_ShortEndianConvert (nData [1]) \
	| TUtil_ShortEndianConvert (nData [2]) | TUtil_ShortEndianConvert (nData [3]); 
}


inline void TUtil_EndianConvertion (uint16_t* pData, unsigned int nDCount)
{
	while (nDCount--)  pData [nDCount] = TUtil_ShortEndianConvert (pData [nDCount]);
}



inline bool Util_Base64Encode (uint8_t* pszDataRaw, uint8_t* pszReturnBase64)
{
	pszReturnBase64 [0] = szBase64Dic [pszDataRaw [0] >> 2];
	pszReturnBase64 [1] = szBase64Dic [(uint8_t) ((pszDataRaw [0] << 6) | (pszDataRaw [1] >> 2)) >> 2];
	pszReturnBase64 [2] = szBase64Dic [(uint8_t) ((pszDataRaw [1] << 4) | (pszDataRaw [2] >> 4)) >> 2];
	pszReturnBase64 [3] = szBase64Dic [(uint8_t) ((pszDataRaw [2] << 2)) >> 2];
	pszReturnBase64 [4] = '\0';
	
	////TRACE  (" Data: [%-.2d] [%-.2d] [%-.2d] [%-.2d]\n", pszReturnBase64 [0], pszReturnBase64 [1], pszReturnBase64 [2], pszReturnBase64 [3]);
	return true;
}


inline uint8_t Util_GetBase64RawValue ( uint8_t nBase64)
{
	if (nBase64 >= 'A' && nBase64 <= 'Z') return nBase64 - 'A';
	else if (nBase64 >= 'a' && nBase64 <= 'z') return nBase64 - 'a' + 26;
	else if (nBase64 >= '0' && nBase64 <= '9') return nBase64 - '0' + 52;
	else if (nBase64 == '+') return 62;
	else if (nBase64 == '/') return 63;
	else  
	{ 
		TRACE ("Trying to use [%u] [%c]\n", nBase64, nBase64); 
		throw ("UTIL Base64 decoding no standard base encoding dictionary character."); 
	}
    
    return 0;
}


inline bool Util_Base64Decode (uint8_t* pszDataRaw, uint8_t* pszReturnBase64)
{

	pszReturnBase64 [0] = Util_GetBase64RawValue (pszReturnBase64 [0]);
	pszReturnBase64 [1] = Util_GetBase64RawValue (pszReturnBase64 [1]);
	pszReturnBase64 [2] = Util_GetBase64RawValue (pszReturnBase64 [2]);
	pszReturnBase64 [3] = Util_GetBase64RawValue (pszReturnBase64 [3]);

	////TRACE  (" Data: [%-.2d] [%-.2d] [%-.2d] [%-.2d]\n", pszReturnBase64 [0], pszReturnBase64 [1], pszReturnBase64 [2], pszReturnBase64 [3]);

	pszDataRaw [0] = ((pszReturnBase64 [0] << 2) | (pszReturnBase64 [1] >> 4));
	pszDataRaw [1] = ((pszReturnBase64 [1] << 4) | (pszReturnBase64 [2] >> 2));
	pszDataRaw [2] = ((pszReturnBase64 [2] << 6) | pszReturnBase64 [3]);
	pszDataRaw [3] = '\0';
	
	////TRACE  (" Data: [%-.2u] [%-.2u] [%-.2u]\n", pszDataRaw [0], pszDataRaw [1], pszDataRaw [2]);
    
    return true;
}


long int Util_DecodeFromBase64 (const char* pszBase64String, const long int nB64StrLen, char* pszRetString, const long int nRetStringMaxLen)
{
	long int nCount;
	uint8_t  nDataPiece [6];
	uint8_t  nB64Decoded [5];
	//long int nReturn = 0;
	
	Verify (pszBase64String != NULL, "The Given pszBase64String is NULL.", -1);
	Verify (pszRetString != NULL,    "The Given pszRetString is NULL.", -1);
	Verify ((nB64StrLen - 1) % 4 == 0 && pszBase64String [nB64StrLen - 1] == '=', "The given Base64 String didn't seam to be compliance.", -2);
	
	for (nCount = 0; nCount < nB64StrLen - 1; nCount += 4)
	{
		memcpy ((void*) nDataPiece, &pszBase64String [nCount], 4);
		Util_Base64Decode (nB64Decoded, nDataPiece);
		
		memcpy (&pszRetString [(nCount / 4) * 3], (char*) nB64Decoded, 4);
	}
	
	TRACE ("Returned: [%s]\n", pszRetString);

	return (nCount / 4) * 3;
}



long int Util_EncodeToBase64 (const char* pszString, const long int nStrLen, char* pszRetString, const long int nRetStringMaxLen)
{
	long int	nCount;
	uint8_t		nEncodeData [5];
	uint8_t		nStrData [5] = {1,2,3,4};
	long int		nStrOffSet;
	long int	nReturn;
	
	Verify (pszString != NULL, "The given pszString is NULL.", -1);
	Verify (pszRetString != NULL, "The given pszRetString is NULL.", -2);
	Verify (nRetStringMaxLen >= ((int) (nStrLen * 1.35)), "The given Return String Len is less than 35% gratter nStrLen.", -3);
	
	for (nCount=0; nCount < nStrLen; nCount += 3)
	{
		nStrOffSet = nCount + 3 > nStrLen ? nStrLen - nCount : 3;
		
		//Next 4 line exist for performance and security reasons instead memset call
		nStrData [0] = 0;
		nStrData [1] = 0;
		nStrData [2] = 0;
		nStrData [3] = 0;
		
		memcpy ((char*) nStrData, (const char*) pszString + nCount, nStrOffSet);

		Util_Base64Encode (nStrData, nEncodeData);
		
		memcpy ((char*) pszRetString + ((long int) (nCount/3) * 4), nEncodeData, 4);
		pszRetString [(long int) ((nCount/3) * 4) + 4] = '\0';		
	}
	
	nReturn = (long int) ((nCount/3) * 4);
	
	pszRetString [(long int) nReturn] = '=';
	pszRetString [(long int) nReturn + 1] = '\0';
	
	TRACE (" Sampling: [%ld] RET: [%s] \n", nReturn, pszRetString);

	return nReturn + 1;
}



/* Codigo */

#ifndef WIN32



void _beginthread (void* (*pFunc)(void*),int _null, void* pArg)
{
    pthread_t tid;
    
    _null=1;
    pthread_create (&tid, NULL, pFunc, pArg);
}

void _endthread(void)
{        
    pthread_exit (NULL);
}


pthread_t __threadid(void)
{
    return pthread_self ();
}


#else
//WINDOWS COMPATIBILITY IMPLEMENTATION

struct tms 
{
	clock_t tms_utime;
	clock_t tms_stime;
	clock_t tms_cutime;
	clock_t tms_cstime;	
};


//WINBASEAPI BOOL WINAPI GetSystemTimes(LPFILETIME,LPFILETIME,LPFILETIME);

clock_t times (struct tms* buffer)
{
	clock_t nReal;
	
	uint64_t idleTime = 0;
	uint64_t kernelTime = 0;
	uint64_t userTime = 0;
#ifndef WIN2000
	Verify (GetSystemTimes ((FILETIME*)&idleTime, (FILETIME*)&kernelTime, (FILETIME*)&userTime) == true, "", 0);
#endif
	
	return (idleTime + kernelTime  * CLK_TCK); //(kernelTime + userTime * CLK_TCK);
}

#endif

/* 
  Trimming procedures will treat any character lower ' ' (32)
  this will work on cases like tab , backspace and bell takes place
  and assuming the given string array is actually a string rathern
  them a binary data, thus be really carefully about using it, if
  your system uses binary array instead.
 
  WORNING: The size of the string must be at least nLen + 1 (1 is '\0' string terminate)
*/


bool TUtil_TrimString (char* pszString, int nLen)
{
    Verify (pszString != NULL, "pszString is NULL, invalid.", false);
    Verify (nLen > 0, "Error, nLen == 0, must be gratter than zero.", false);

    //uint nStart, nStop;
    int nCount;

    /* Trimming leftwards first */
    nCount = nLen - 1;

    while (pszString [nCount] <= ' ' && nCount >= 0) nCount--;

    if (nCount < 0)
    {
        pszString [0] = '\0';

        return true;
    }
    else
    {
        pszString [nCount + 1] = '\0';
    }

    nCount = 0;

    while (pszString [nCount] <= ' ' && nCount < nLen && pszString [nCount] != '\0') nCount++;

    Verify (nCount != nLen, "Error, it couldn't have riched nLen value if the leftwards procedures was ok.", false);

    uint nOnSet = 0;
    for (;nCount < nLen && pszString [nCount] != '\0'; nCount++)
    {
        pszString [nOnSet++] = pszString [nCount];
    }

    pszString [nOnSet] = '\0';

    return true;
}


bool TUtil_TrimString_ (char* pszString, int nLen)
{
    int   nCount;
    bool  bInfo;
    
    Verify (pszString != NULL, "", false);
    Verify (nLen > 0, "", false);
    
    bInfo = true;
    
    for (nCount = nLen - 1; nCount >= 0; nCount--)
    {
        if (pszString [nCount] != ' ')
        {
            pszString [nCount + 1] = '\0';
            break;
        }
    }
 
    if (nCount == 0 && pszString [nCount] == ' ')
    {
        pszString [0] = '\0';
    }
    
    for (nCount = 0; nCount < nLen && pszString [nCount] != '\0'; nCount++)
    {
        if (pszString [nCount] != ' ')
        {
            if (nCount > 0)
            {
                Verify (strncpy (pszString, (const char*) &pszString [nCount], nLen - nCount) != NULL, "", false);
				
				pszString [nLen - nCount] = '\0';
            }
            
            break;
        }
    }
    
    return true;
}



bool TUtil_ReplaceChar (char* pszString, int nLen, char chCharIn, char chCharOut)
{
    int nCount;
    
    for (nCount=0; nCount < nLen; nCount++)
    {
        if (pszString [nCount] == chCharIn)
        {
            pszString [nCount] = chCharOut;
        }
    }
    
    return true;
}



bool TUtil_ValidateMask (const char* pszMask, const char* pszString, int nLen)
{
	bool bSearch;
	
	Verify (pszMask != NULL, "Mascarra nao informada.", false);
	Verify (pszString != NULL, "String informada nula.", false);
	
	if (pszString [0] == '\0')
	{
		return false;
	}
	
	bSearch = false;
	
	while (pszMask [0] != '\0')
	{

		if (pszMask [0] == '*')
		{
			pszMask++;
			
			if (pszMask [0] == '\0')
			{
				return true;
			}
			
			bSearch = true;
			
			continue;
		}

		while (pszString [0] != '\0')
		{
			if (bSearch == false && pszString [0] != pszMask [0])
			{
				return false;
			}
			else if (pszString [0] == pszMask [0])
			{
				if (bSearch == true )
				{
					bSearch = false;
				}
				
				pszString++;
				break;
			}
			
			pszString++;
		}
		
		if (pszString [0] == '\0')
		{
			if (bSearch == true)
			{
				return false;
			}
			else
			{
				break;
			}
		}
		
		pszMask++;
	}

	if (pszMask [0] == '\0' && pszString [0] != '\0')
	{
		return false;
	}
	
	return true;
}




void TUtil_StripEOL (char* pszString, int nLen)
{	
	while (nLen >= 0 && pszString [nLen] < ' ') nLen--;
	
	if (nLen >= 0) pszString [nLen + 1] = '\0';
}




uint32_t TUtil_CountToken (const char chToken, const char* pszString, uint32_t nStrLen)
{
    int32_t nCount;
    
    nCount = 1;
    
    while (--nStrLen > 0)
    {
        if (pszString [nStrLen - 1] == chToken)
        {
            /*
			while (nStrLen - 1 >= 0 && pszString [nStrLen - 1] == ' ')
				nStrLen--;
			*/
            
            nCount++;
        }
    }
    
    return nCount;
}


/*
   TUtil_GetToken
 
 Used to return a value from 0-n, where 0 is the first item, 1 the second and so on.
 
 The nStrLen is the size of the pszReturn, but the function will use strlen to
 discover the size of pszString, so be really carefully about the given string, if it has
 alredy been ended with '\0'

*/

char* TUtil_GetToken (char chToken, const char* pszString, char* pszReturn, long int nStrLen, int nToken)
{
    bool       nInfo;
    long int   nCount;
    char       chItem;
    long int   nCountToken;
    long int   nCountChar;
    long int   nLen;

    Verify (pszString != NULL, "String fornecida esta nula.", NULL);
	Verify (nStrLen > 0      , "Tamanho da string invalida.", NULL);
	Verify (pszReturn != NULL, "String de retorno nula."    , NULL);
	
	/* This functions uses 0 as the first intem. */
	
    nInfo       = (nToken == false);
    nCountToken = 0;
    nCountChar  = 0;
    
    nLen = strlen (pszString);
    pszReturn [0] = _EOS;
    
    for (nCount = 0; nCount < nLen; nCount++)
    {
        chItem = pszString [nCount];
        
		if (chItem == chToken)
		{
			if (chToken == ' ')
				while (pszString [nCount + 1] == chToken && nCount + 1 < nLen) nCount++; 
					   
			if (nInfo == false)
			{
				nCountToken++;
				if (nCountToken == nToken || nToken == 0)
				{
					nInfo = true;
				}
			}
			else if (nInfo == true && chItem == chToken)
			{
				return pszReturn;
			}
		}
        else if (nInfo == true)
        {
            pszReturn [nCountChar] = chItem;
            nCountChar++;
            pszReturn [nCountChar] = _EOS;
            
            if (nCountChar >= nStrLen)
            {
                return pszReturn;
            }
        }
    }
    
	if (nInfo == false)
	{
		return NULL;
	}
	
	return pszReturn;
}




bool TUtil_Crypt (unsigned char* pszKey, unsigned char* pszData, size_t nSize)
{
    unsigned char* pszTemp;
    
    pszTemp = pszKey;
    while (nSize-- > 0)
    {        
        *pszData =  *pszData ^ *pszKey;
        
        pszData++;
        pszKey++;
        if (*pszKey == '\0')
        {
            pszKey = pszTemp;
        }
    }
    
    return true;
}





bool TUtil_GenerateRandomKey (char* pszKey, int nLen)
{
    float fRand;
    Verify (pszKey != NULL, "Chave informada invalida.", false);
    
    srand ((unsigned int) time (NULL));
    
    pszKey [0] = '\0';
    
    pszKey [nLen] = '\0';
    
    while (--nLen >= 0)
    {
        fRand = (float) rand () / (RAND_MAX);
        
        pszKey [nLen] = (char) (fRand * 90) + ' ';
    }
        
    return true;
}



ssize_t TUtil_WriteCript (char* pszKey, int nFd, void* pData, size_t nSize)
{
    ssize_t nReturn;
    
    TUtil_Crypt ((unsigned char*) pszKey, (unsigned char*) pData, nSize);
    
    nReturn = write (nFd, pData, nSize);
    
    TUtil_Crypt ((unsigned char*) pszKey, (unsigned char*) pData, nSize);
    
    return nReturn;
}




ssize_t TUtil_ReadCript (char* pszKey, int nFd, void* pData, size_t nSize)
{
    ssize_t nReturn;

    if ((nReturn = read (nFd, pData, nSize)) <= 0)
    {
        return nReturn;
    }
    
    TUtil_Crypt ((unsigned char*) pszKey, (unsigned char*) pData, nSize);
    
    return nReturn;
}


bool TUtil_Uppcase (char* pszString)
{
    while (*pszString != '\0')
    {
        *pszString = toupper (*pszString);
        pszString++;
    }
    
    return true;
}



bool TUtil_GetValueFromResponse (const char* pszSource, const char* pszField, char* pszData, int nDataLen)
{
	return  TUtil_GetValueFromResponse2 (pszSource, pszField, pszData, nDataLen, '&', '=');
}



bool TUtil_GetValueFromResponse2 (const char* pszSource, const char* pszField, char* pszData, int nDataLen, char chDelimiter, char chDataDv)
{
    int    nCount;
    //char   pszTmpField [UTIL_FIELDLEN + 1];
    int    nType;
    bool   bMatch;
    
    nType  = 0;
    nCount = 0;
    
    bMatch = true;
    while (*pszSource != '\0')
    {
        if (*pszSource != chDelimiter)
        {
            if (nType == 0 && nCount < UTIL_FIELDLEN)
            {
                if (*pszSource != chDataDv && bMatch == true)
                {
                    if (*pszSource == pszField [nCount])
                    {
                        bMatch = true;
                    }
                    else
                    {
                        bMatch = false;
                    }
                    
                    nCount++;
                }
                else if (bMatch == true && *pszSource == chDataDv)
                {
                    nType  = 1;
                    nCount = 0;
                }
            }
            else if (nType == 1)
            {
                if (nCount < nDataLen)
                {
                    pszData [nCount] = *pszSource;
                    nCount++;
                } 
            }
        }
        else if (bMatch == true)
        {
            pszData [nCount] = '\0';
            return true;
        }
        else
        {
            nType  = 0;
            nCount = 0;
            bMatch = true;
        }
        
        pszSource++;
    }
    
    return false;
}



bool TUtil_GetValueFromResponse3 (const char* pszSource, const char* pszField, char* pszData, int nDataLen, char chDelimiter, char chDataDv)
{
    int    nCount;
    //char   pszTmpField [UTIL_FIELDLEN + 1];
    int    nType;
    bool   bMatch;
	bool   bString;
    
    nType  = 0;
    nCount = 0;
    
    bMatch  = true;
	bString = false;
	
    Verify (pszSource != NULL, "Error, the given pszSource is NULL.\n", false);
    
    while (*pszSource != '\0')
    {
        if (*pszSource != chDelimiter)
        {
            if (nType == 0 && nCount < UTIL_FIELDLEN)
            {
                if (*pszSource != chDataDv && bMatch == true)
                {
                    if (*pszSource == pszField [nCount])
                    {
                        bMatch = true;
                    }
                    else
                    {
                        bMatch = false;
                    }
                    
                    nCount++;
                }
                else if (bMatch == true && *pszSource == chDataDv)
                {
                    nType  = 1;
                    nCount = 0;
                }
            }
            else if (nType == 1)
            {
				if (*pszSource == '\'')
				{
					if (bString == false)
					{
						bString = true;
					}
					else
					{
						bString = false;
					}
				}
				else if (nCount < nDataLen)
                {
                    pszData [nCount] = *pszSource;
                    nCount++;
                } 
            }
        }
        else if (bMatch == true && bString == false)
        {
            return true;
        }
        else
        {
            nType  = 0;
            nCount = 0;
            bMatch = true;
        }
        
        pszSource++;
    }
    
    return false;
}




bool TUtil_TokennedUppcase (char* pszString, char szToken)
{
    while (*pszString != '\0' && *pszString != szToken)
    {
        *pszString = toupper (*pszString);
        pszString++;
    }
    
    return true;
}


void TUtil_NanoSleep (long int nTime, long int nTime2)
{
#ifndef WIN32
    struct timespec time1;
    struct timespec time2 = { 0, 0};
    
    time1.tv_nsec = nTime2;
    time1.tv_sec  = nTime;
    
    Verify (nanosleep (&time1, &time2) >= 0, "", );
	
	TRACE ("Tempo de sleep: %lu\n", time2.tv_nsec);
#else
     sleep (nTime * 1000);
     sleep (nTime2 / 1000);
#endif
}


#ifndef _NOMD5

bool TUtil_MD5hex (const char *src, char *hex_output)
{
    md5_state_t state;
    md5_byte_t digest[16];
    int nCount;

    md5_init (&state);
	
    Verify (md5_append (&state, (const md5_byte_t *) src, (int) strlen (src)) == true, "Erro processing MD5 encryptation.", false);
	
    Verify (md5_finish (&state, digest) == true, "Error finalising MD5 encryptation.", false);
	
    for (nCount = 0; nCount < 16; ++nCount)
        sprintf (hex_output + nCount * 2, "%02X", digest[nCount]);
		
	return true;
}

#endif


uint64_t TUtiL_crc64_v2 (const char *seq, uint64_t crc)
{
    int i, j;
    uint64_t  part;
    static int init = 0;
    static unsigned long long CRCTable[256];
    
    if (!init)
    {
		init = 1;
		for (i = 0; i < 256; i++)
		{
			part = i;
			for (j = 0; j < 8; j++)
			{
			if (part & 1)
				part = (part >> 1) ^ POLY64REV;
			else
				part >>= 1;
			}
			CRCTable[i] = part;
		}
    }
    
    while (*seq)
	crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8);

    /* 
     The output is done in two parts to avoid problems with 
     architecture-dependent word order
     */

    return crc;
}



uint64_t TUtiL_crc64Base2Binary (const char *seq, long int nLen, uint64_t crc)
{
    int i, j;
    uint64_t  part;
    static int init = 0;
    static unsigned long long CRCTable[256];
    
    if (!init)
    {
		init = 1;
		for (i = 0; i < 256; i++)
		{
			part = i;
			for (j = 0; j < 8; j++)
			{
				if (part & 1)
					part = (part >> 1) ^ ((long long int) ((POLY64REV ^ 1223465324LL) << 16) & POLY64REV);
				else
					part >>= 1;
			}
			CRCTable[i] = part;
		}
    }
    
    while (--nLen >= 0)
	{ crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8); }
	
    /* 
     The output is done in two parts to avoid problems with 
     architecture-dependent word order
     */
	
	////TRACE ("CRC: [%-.10llX]\n", crc);
	
    return crc;
}




uint64_t TUtiL_crc64Base3Binary (const char *seq, long int nLen, uint64_t crc)
{
    int i, j;
    uint64_t  part;
    static int init = 0;
    static unsigned long long CRCTable[256];
    
    if (!init)
    {
		init = 1;
		for (i = 0; i < 256; i++)
		{
			part = i;
			for (j = 0; j < 8; j++)
			{
				if (part & 1)
					part = (part >> 1) ^ ((long long int)  ((POLY64REV ^ 33298550399123LL) << 4) & POLY64REV);
				else
					part >>= 1;
			}
			CRCTable[i] = part;
		}
    }
    
    while (--nLen >= 0)
	{ crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8); }
	
    /* 
     The output is done in two parts to avoid problems with 
     architecture-dependent word order
     */
	
	////TRACE ("CRC: [%-.10llX]\n", crc);
	
    return crc;
}






uint64_t TUtiL_crc64Binary (const char *seq, long int nLen, uint64_t crc)
{
    int i, j;
    uint64_t  part;
    static int init = 0;
    static unsigned long long CRCTable[256];
    
    if (!init)
    {
		init = 1;
		for (i = 0; i < 256; i++)
		{
			part = i;
			for (j = 0; j < 8; j++)
			{
				if (part & 1)
					part = (part >> 1) ^ POLY64REV;
				else
					part >>= 1;
			}
			CRCTable[i] = part;
		}
    }
    
    while (--nLen >= 0)
	{ crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8); }
	
    /* 
     The output is done in two parts to avoid problems with 
     architecture-dependent word order
     */
	
	//TRACE ("CRC: [%-.10llX]\n", crc);
	
    return crc;
}




uint64_t TUtiL_crc64(const char *seq)
{
	return TUtiL_crc64_v2 (seq, INITIALCRC);
}





bool TUtil_IsReady (int nFD, int nTimeoutSeconds, int nTimeoutUSeconds)
{
    fd_set            fdSet;
    struct timeval    tmVal;
    bool              bReturn;
    int               nReturn;

    Verify (nFD >= 0, "Objeto not intialized.", false);
   
    tmVal.tv_sec  = nTimeoutSeconds;
    tmVal.tv_usec = nTimeoutUSeconds;
   
    FD_ZERO (& fdSet);
    FD_SET  (nFD, &fdSet);


#ifndef _HPUX_
    Verify (select (nFD + 1, &fdSet, NULL, NULL, &tmVal) >= 0, "", false);
#else
    Verify (select (nFD + 1,(int*) &fdSet, NULL, NULL, &tmVal) >= 0, "", false);
#endif

    Verify ((nReturn = FD_ISSET (nFD, &fdSet)) >=0, "", false);

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


#ifndef WIN32

#  ifndef CLK_TCK
#   define CLK_TCK      CLOCKS_PER_SEC
#  endif

void Util_CPU_Init ()
{    
    m_nReal = times (&m_stTimes) / CLK_TCK;
}



void Util_CPUReduce ()
{
	static long int nCount = 0;
	
	if (nCount++ % nMaxCPUSteps == 0)
#ifndef WIN32
	usleep ((useconds_t) nGlobalSleep);
#else
	sleep (nGlobalSleep);
#endif
}



double Util_Show_CPU_Usage (bool bChild)
{
    static bool bInit = true;
    static struct tms mstTimes;
    static clock_t mnReal = 0;

    static long int clktck = 0;

    if (clktck == 0) clktck = sysconf(_SC_CLK_TCK);

	clock_t    nReal = 0;
    clock_t    nUser = 0;
    clock_t    nSystem = 0;
    clock_t    nTime = 0;
    struct  tms          stTimes;

    static  double       dPorcentage = 0;


    if (bInit == false)
    {
        nTime = times (&stTimes); // clktck;

        nReal = nTime - mnReal;

        if (bChild == false)
        {
            nUser    = stTimes.tms_utime;// - mstTimes.tms_utime;
            nSystem  = stTimes.tms_stime;// - mstTimes.tms_stime;
        }
        else
        {
            nUser    = stTimes.tms_cutime;// - mstTimes.tms_cutime;
            nSystem  = stTimes.tms_cstime;// - mstTimes.tms_cstime;
        }

        if (nReal > 0)
        {
            dPorcentage = (double) (nUser + nSystem) * 100.0;
            dPorcentage /= nTime;
        }
        else
            dPorcentage = 0;


        TRACE ("CPU: %.1f%% ( (usr)[%lu] (sys)[%lu] - (Time)[%lu] - (Real)[%lu] )  \r", dPorcentage, nUser, nSystem, nTime, nReal);

        //TRACE ("CPU: %.2f%%  CLK_TCK: [%u]  \n\n", dPorcentage, nReal, stTimes.tms_utime, stTimes.tms_stime, CLOCKS_PER_SEC);
    }
    else
    {
        bInit = false;
    }


	mnReal = nTime;
    mstTimes = stTimes;

	return dPorcentage;
}



double Util_CPU_GetUsage (bool bChild)
{
	unsigned long int    nReal;
    unsigned long int    nUser;
    unsigned long int    nSystem;
    struct  tms          stTimes;
    static  double       dPorcentage = 0;
	
	
	nReal = (times (&stTimes) / CLK_TCK) - m_nReal;
	
	if (bChild == false)
	{
		nUser    = stTimes.tms_utime - m_stTimes.tms_utime;
		nSystem  = stTimes.tms_stime - m_stTimes.tms_stime;
	}
	else 
	{
		nUser    = stTimes.tms_cutime - m_stTimes.tms_cutime;
		nSystem  = stTimes.tms_cstime - m_stTimes.tms_cstime;
	}
	
	if (nReal > 0)
        dPorcentage = (double) (nUser + nSystem) / nReal;
    else
        dPorcentage = 0;
	
    TRACE ("CPU: %f%% (%lu, %lu, %lu) Type: [%s] \n\n", dPorcentage, nReal, stTimes.tms_utime, stTimes.tms_stime, bChild == true ? "Child" : "Father");
	
	Util_CPU_Init ();

	return dPorcentage;	
}




double Util_CPU_Usage2 (bool bChild)
{
    unsigned long int    nReal;
    unsigned long int    nUser;
    unsigned long int    nSystem;
    struct  tms          stTimes;
    static  double       dPorcentage = 0;

	
	nReal = (times (&stTimes) / CLK_TCK) - m_nReal;

	if (bChild == false)
	{
		nUser    = stTimes.tms_utime - m_stTimes.tms_utime;
		nSystem  = stTimes.tms_stime - m_stTimes.tms_stime;
	}
	else 
	{
		nUser    = stTimes.tms_cutime - m_stTimes.tms_cutime;
		nSystem  = stTimes.tms_cstime - m_stTimes.tms_cstime;
	}

    if (nReal > 0) dPorcentage = (double) (nUser + nSystem) / nReal;

    printf ("CPU: %f%% (%lu, %lu, %lu) Type: [%s] \n\n", dPorcentage, nReal, stTimes.tms_utime, stTimes.tms_stime, bChild == true ? "Child" : "Father");
	
    if (nReal >= 2 && nMaxCPUUsage > 0)
    {		
        TRACE ("CPU USAGE: Total: %lu User: %lu System: %lu  Porcetage used: %2.2f %%\n", 
			   nReal, nUser, nSystem,  dPorcentage);
        
        if (dPorcentage > nMaxCPUUsage + 5)
        {
			if (nMaxCPUSteps > 50)
			{
				if ((nMaxCPUSteps -= 30) <= 0)
					nMaxCPUSteps = 1;
				
				TRACE ("CPU USAGE: Decreasing sleeping time...\n");
			}
			else 
			{
				nGlobalSleep += (USLEEPRESOLUTION);
				nMaxCPUSteps = 200;

				TRACE ("CPU USAGE: Trying to reduce CPU usage increasing everage sleeping... (%lu)\n", nGlobalSleep);
			}
        }
		else if (dPorcentage <= nMaxCPUUsage - 4 && nGlobalSleep > 2)
		{
			if (nMaxCPUSteps  < 500)
			{
				nMaxCPUSteps += 77;
				TRACE ("CPU USAGE: Trying to achive the max performance by everage sleeping\n");
			}
			else if (nGlobalSleep > 0)
			{
				TRACE ("CPU USAGE: Trying to achive the max performance by numbers of sleeping times\n");
				nGlobalSleep -= 2;
				nMaxCPUSteps = 200; 
			}
		}
		/*
		else if (nGlobalSleep > 200)
		{
           nGlobalSleep = 0;    
        }
		*/
		
        Util_CPU_Init ();
    }
    else
    {
        TRACE ("Real: %lu\n", nReal);
    }

	TRACE ("CPU USAGE: Max: %lu - Global Everage: %lu Step: %lu\n", nMaxCPUUsage, nGlobalSleep, nMaxCPUSteps);

    return dPorcentage;
}




double Util_CPU_Usage ()
{
	return Util_CPU_Usage2 (0);
}




double Util_CPU_UsagebyChild ()
{
	return Util_CPU_Usage2 (true);
}



void Util_SetMaxCPU (unsigned long int nMax)
{
	nMaxCPUUsage = nMax;
}

#endif


clock_t Util_GetClock (void)
{
#ifndef WIN32
	//struct tms tms;
	//return times (&tms) * (CLOCKS_PER_SEC); // / CLK_TCK);
	return clock ();
#else
	return clock ();
#endif
}




/* chksum_crc() -- to a given block, this one calculates the
 *				crc32-checksum until the length is
 *				reached. the crc32-checksum will be
 *				the result.
 */
uint32_t Util_CRC32v2 (unsigned char *block, unsigned int length, uint32_t crc_start)
{
	uint32_t crc;
	uint32_t i;
	static bool bTable = false;
	
	if (bTable == false)
	{
		uint32_t crc, poly;
		int i, j;
		
		poly = 0xEDB88320L;
		for (i = 0; i < 256; i++)
		{
			crc = i;
			for (j = 8; j > 0; j--)
			{
				if (crc & 1)
				{
					crc = (crc >> 1) ^ poly;
				}
				else
				{
					crc >>= 1;
				}
			}
			crc_tab[i] = crc;
		}		
	}
	
	
	crc = crc_start == 0 ? 0xFFFFFFFF : crc_start;
	
	for (i = 0; i < length; i++)
	{
		crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
	}
	return (crc ^ 0xFFFFFFFF);
}



	
void UTil_PrintDataToDebug (uint8_t* szSectionData, long int nDataLen)
{
	long int nCount;
	long int nCount1;
	long int nLen;
	char szPData [20];
	
#ifdef _DEBUG
	printf ("%s : Total Visualizing: [%-8lu]\n", __FUNCTION__, nDataLen);
    printf ("%s :       ADDRESS       00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15  [    DATA  RAW   ]\n", __FUNCTION__);
    printf ("%s : ------------------- |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  -------------------\n", __FUNCTION__);

	for (nCount=0; nCount < nDataLen; nCount = nCount + 16)
	{
		nLen = nCount + 16 > nDataLen ? nDataLen - nCount : 16;
		
		printf ("%s : Addr: [%-.10lu] ", __FUNCTION__, nCount);
		for (nCount1=0; nCount1 < 16; nCount1++)
		{
			if (nCount1 + nCount < nDataLen)
			{
				printf ("%-.2X ", (uint8_t) szSectionData [nCount + nCount1]);
				szPData [nCount1] = szSectionData [nCount + nCount1] < 32 || szSectionData [nCount + nCount1] >= 127 || szSectionData [nCount + nCount1] == '>' || szSectionData [nCount + nCount1] == '<' ? '.' : szSectionData [nCount + nCount1];
			}
			else 
			{
				printf (".. "); szPData [nCount1] = '.';
			}

		}
		
		szPData [nCount1] = '\0';
		
		printf ("  [%s]\n", szPData);
	}	
	
    printf ("CHECKSUM  [%X-%X]\n", Util_CRC32v2 (szSectionData, (unsigned int) nDataLen, (uint32_t) 0xFFFF), Util_CRC32v2 (szSectionData, (unsigned int) nDataLen, (uint32_t) 0x0));

    fflush (stdout);
#endif
}




bool Util_HTTPDataDecode (char* pszData, int nDataLen)
{
	unsigned long int   nCount  = 0;
	unsigned long int   nStrCount=0;
	unsigned int nDecodedChar;
	char chDecChar [2] = {0,0};
	
	for (nCount=0; nCount<= nDataLen; nCount++)
	{
		if (pszData [nCount] == '%')
		{
			if (nCount + 2 < nDataLen)
			{
				chDecChar [0] = pszData [++nCount];
				chDecChar [1] = pszData [++nCount];
				
				nDecodedChar = (HexVal (chDecChar [0]) * 16) + HexVal (chDecChar [1]);
				pszData [nStrCount++] = nDecodedChar;
			}
			else
			{
				break;
			}
		}
		else
		{
			pszData [nStrCount++] = pszData [nCount];
		}				
	}
	
	pszData [nStrCount] = '\0';
	
	return true;
} 



bool Util_HTTPDataEncode (char* pszData, int nDataLen, char* pszReturn, int nMaxStringLen)
{
	unsigned long int   nCount  = 0;
	unsigned long int   nStrCount=0;
	
	Verify (pszData   != NULL, "The pszData Objtect is invalid.", false);
	Verify (nDataLen  > 0    , "The Size of Data must be greater then zero", false);
	Verify (nMaxStringLen > 0, "The Size of MaxData must be greater then zero.", false);
	Verify (nMaxStringLen > nDataLen, "The Size of MaxData must be greater then nDataLen.", false);
	Verify (pszReturn != NULL,   "The pszReturn must be supplied to be used as the return data.", false);
	
	for (nCount=0; nCount < nDataLen; nCount++)
	{
		if (isalpha (pszData [nCount]) || isdigit (pszData [nCount]))
		{
			pszReturn [nStrCount++] = pszData [nCount];
		}
		else if (nStrCount + 3 < nMaxStringLen)
		{
			//snprintf (&pszReturn [nStrCount], 3, "%%%-.2x", pszData [nCount]);
			pszReturn [nStrCount++] = '%';
			pszReturn [nStrCount++] = HexChar (((int8_t)(pszData [nCount] / 16)));
			pszReturn [nStrCount++] = HexChar (((int8_t)(pszData [nCount] % 16)));
		}
		else
		{
			break;
		}
		
		pszReturn [nStrCount] = '\0'; 
	}		
    
	strncpy (pszData, pszReturn, nStrCount);
	pszData [nStrCount] = '\0';
		
	return true;
}



void Util_PrintBinary (uint8_t chData)
{
    int nCount;
    
	TRACE ("Value: %u (%c) : ", (uint8_t) chData, chData);
    
    for (nCount=1; nCount < 256; nCount *= 2)
    {
		if (nCount > 1 && nCount < 256) fprintf(stderr, ("."));
		fprintf  (stderr, "%u", (chData & nCount) == 0 ? 0 : 1);
    }
    fprintf (stderr, "\n");
	fflush (stderr);
}



const char* TUtil_GetPtrFromToken (char chToken, const char* pszText, uint32_t nTextSize, uint16_t nToken)
{
    if (nToken == 0)
        return pszText;
    
    //TRACE ("nTextSize: [%u] nToken: [%u]\n", nTextSize, nToken);
    
    for (uint32_t nCount = 0; nCount < nTextSize; nCount++)
    {
        if (pszText [nCount] == chToken)
        {
            if (--nToken == 0)
            {
                if (nCount + 1 == nTextSize)
                {
                    return "";
                }
                else
                {
                    return (const char*) &pszText [nCount];
                }
            }
        }
    }
    
    return NULL;
}



char* TUtil_GenerateUID (const char* pszData, uint32_t nDataSize, char* pszReturn, uint32_t nRetLen)
{
    Verify (pszData != NULL, "Error, Data is NULL.", NULL);
    Verify (pszReturn != NULL, "Error, Return is NULL.", NULL);
    
    typedef union _datablk
	{
		uint64_t _data;
		uint16_t Values16b [4];
		uint32_t Values32b [2];
		uint8_t	 Values8b  [8];
	} DataBlk64;
	
	DataBlk64 i64Deck [3];
    
    i64Deck [0]._data = TUtiL_crc64Binary (pszData, nDataSize, (uint64_t) 120);
    i64Deck [1]._data = TUtiL_crc64Binary (pszData, nDataSize, (uint64_t) i64Deck [0]._data ^ 0x0A0A344A1LLU);
    i64Deck [2]._data = TUtiL_crc64Base2Binary (pszData, nDataSize, (uint64_t) i64Deck [0]._data ^ 0xF4589AB2FLLU);
    
    
    //TRACE ("\n\tCRC1: [%llX]\n\tCRC2: [%llX]\n\tCRC3: [%llX]\n", i64Deck [0]._data, i64Deck [1]._data, i64Deck [2]._data);
           
    snprintf (pszReturn, nRetLen, "%08X-%04X-%04X-%04X-%04X-%08X", 							
              i64Deck [1].Values32b [0],
              i64Deck [2].Values16b [0], 
              i64Deck [2].Values16b [1], 
              i64Deck [2].Values16b [2], 
              i64Deck [2].Values16b [3],
              i64Deck [1].Values32b [1]);

    return pszReturn; 
}

uint64_t TUtil_GenerateAssign (const char* pszData, uint32_t nDataSize, char* pszReturn, uint32_t nRetLen)
{
    Verify (pszData != NULL, "Error, Data is NULL.", NULL);
    Verify (pszReturn != NULL, "Error, Return is NULL.", NULL);
    
    typedef union _datablk
	{
		uint64_t _data;
		uint16_t Values16b [4];
		uint32_t Values32b [2];
		uint8_t	 Values8b  [8];
	} DataBlk64;
	
	DataBlk64 i64Deck [3];
    
    i64Deck [0]._data = TUtiL_crc64Binary (pszData, nDataSize, (uint64_t) 120323LLU);
    i64Deck [1]._data = TUtiL_crc64Binary (pszData, nDataSize, (uint64_t) i64Deck [0]._data ^ 0x0A04A1LLU);
    i64Deck [2]._data = TUtiL_crc64Base2Binary (pszData, nDataSize, (uint64_t) i64Deck [0]._data ^ 0xF458934AFLLU);
    
    
    //TRACE ("\n\tCRC1: [%llX]\n\tCRC2: [%llX]\n\tCRC3: [%llX]\n", i64Deck [0]._data, i64Deck [1]._data, i64Deck [2]._data);
    
    snprintf (pszReturn, nRetLen, "%08X-%04X-%04X%04X-%04X-%08X", //36posicoes.
              i64Deck [1].Values32b [0],
              i64Deck [2].Values16b [0], 
              i64Deck [2].Values16b [1], 
              i64Deck [2].Values16b [2], 
              i64Deck [2].Values16b [3],
              i64Deck [1].Values32b [1]);
    
    return i64Deck [0]._data; 
}


bool TUtil_AssignString (string& strData)
{
    char szData [50];
    
    TUtil_GenerateAssign (strData.c_str(), (uint32_t) strData.length(), szData, sizeof (szData)-1);
    
    strData = strData + ":";
    strData.append(szData, strlen (szData));
    
    return true;
}


bool TUtil_CheckAssignedString (const char* pszData, uint32_t nLength)
{
    Verify (nLength > 36, "The length of the given string must be gratter than 36 characters.", false);

    string strData;

    strData.append (pszData, nLength-37);
    
    TUtil_AssignString (strData);
    
    printf ("GIVEN PSZDATA---------------\n");
    UTil_PrintDataToDebug((uint8_t*) pszData, nLength);
    printf ("Generated-------------------\n");
    UTil_PrintDataToDebug((uint8_t*) strData.c_str(), strData.length());
    
    if (memcmp (strData.c_str(), pszData, strData.length()) != 0)
        return false;
    
    return true;
}



//#define CRC32_GEN	0x04c11db7  /* CCITT standard */
//#define CRC32_MSB	0x80000000

__inline uint32_t crc32_for_byte(uint32_t crc) 
{
    for (int j = 0; j < 8; ++j)
    {
        if (crc & 1) 
        {
            crc = (uint32_t) (crc >> 1) ^ ((uint32_t) 0xEDB88320UL);
        } 
        else {
            crc = (uint32_t) crc >>  1;
        }
    }
    
    //Serial.println (crc);   
    return (uint32_t) crc;
}


uint32_t TUtil_CRC32(const uint8_t *data, size_t n_bytes, uint32_t crc) 
{
    if (data == NULL) return crc;
    
    for(size_t nCount = 0; nCount < n_bytes; ++nCount)
        crc = crc32_for_byte (((uint32_t) crc ^ ((uint8_t*)data)[nCount])  & 0xff) ^ (crc >> 8);
    
    return crc;
}




