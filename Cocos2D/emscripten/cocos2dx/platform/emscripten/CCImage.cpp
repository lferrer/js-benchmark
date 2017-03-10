/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#define __CC_PLATFORM_IMAGE_CPP__
#include "platform/CCImageCommon_cpp.h"

#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "platform/CCImage.h"
#include "platform/CCFileUtils.h"
#include "platform/CCCommon.h"
#include "CCStdC.h"
#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <net/arpa/inet.h>

#define szFont_kenning 2

#define RSHIFT6(num) ((num)>>6)

using namespace std;

struct TextLine {
	std::string sLineStr;
	int iLineWidth;
};

NS_CC_BEGIN

class BitmapDC
{
public:
    BitmapDC()
    {
		iInterval = szFont_kenning;
		m_pData = NULL;
		reset();
    }
    
    ~BitmapDC(void)
    {
    }
    
    void reset() {
		iMaxLineWidth = 0;
		iMaxLineHeight = 0;
		vLines.clear();
	}
    
    void buildLine(std::stringstream& ss, TTF_Font *face)
    {
        TextLine oTempLine;
        ss << '\0';
        oTempLine.sLineStr = ss.str();

        int w, h;
        TTF_SizeText(face, oTempLine.sLineStr.c_str(), &w, &h);

        oTempLine.iLineWidth = w;

        iMaxLineWidth = MAX(iMaxLineWidth, oTempLine.iLineWidth);
        ss.clear();
        ss.str("");
        vLines.push_back(oTempLine);
	}

	bool divideString(TTF_Font *face, const char* sText, int iMaxWidth, int iMaxHeight) {
        std::stringstream ss;
        int w, h;

        // if there is no maximum width specified, just slam the whole input
        // string into a single line, thereby avoiding many expensive calls to
        // compute font metrics.
        if(iMaxWidth == 0)
        {
            std::string text = sText;
            ss << text;
            buildLine(ss, face);
            return true;
        }

        std::vector<std::string> words;
        std::string text = sText;
        std::istringstream iss(text);
        copy(std::istream_iterator<std::string>(iss),
             std::istream_iterator<std::string>(),
             std::back_inserter< vector<string> >(words));

        for(std::vector<string>::iterator i = words.begin(); i != words.end(); ++i)
        {
            // Specially handle the case where a single word exceeds the
            // available width.
            TTF_SizeText(face, i->c_str(), &w, &h);
            if(w > iMaxWidth)
            {
                buildLine(ss, face);
                for(std::string::iterator c = i->begin(); c != i->end(); ++c)
                {
                    ss << *c;
                    TTF_SizeText(face, ss.str().c_str(), &w, &h);
                    if(w > iMaxWidth)
                    {
                        buildLine(ss, face);
                    }
                }
                continue;
            }

            std::string tmp = ss.str() + std::string(" ") + *i;
            TTF_SizeText(face, tmp.c_str(), &w, &h);
            if(w > iMaxWidth)
            {
                buildLine(ss, face);
                ss << *i;
            }
            else
            {
                ss << " " << *i;
            }
        }

        buildLine(ss, face);

        return true;
	}

	/**
	 * compute the start pos of every line
	 *
	 * return >0 represent the start x pos of the line
	 * while -1 means fail
	 *
	 */
    int computeLineStart(CCImage::ETextAlign eAlignMask, int lineWidth, int maxLineWidth)
    {
        int result = 0;
        if( eAlignMask == CCImage::kAlignCenter ||
            eAlignMask == CCImage::kAlignTop ||
            eAlignMask == CCImage::kAlignBottom)
        {
            result = (maxLineWidth / 2) - (lineWidth / 2);
        }
        else if(eAlignMask == CCImage::kAlignRight ||
                eAlignMask == CCImage::kAlignTopRight ||
                eAlignMask == CCImage::kAlignBottomRight)
        {
            result = maxLineWidth - lineWidth;
        }
        // In all other cases (left alignment, most likely), return 0.

        // Attempt to ensure that we don't produce any completely invalid reslts.
        if(result < 0)
        {
            result = 0;
        }
        return result;
    }

    int computeLineStartY(CCImage::ETextAlign eAlignMask, int lineHeight, int maxLineHeight)
    {
        int result = 0;
        if( eAlignMask == CCImage::kAlignCenter ||
            eAlignMask == CCImage::kAlignRight ||
            eAlignMask == CCImage::kAlignLeft)
        {
            result = (maxLineHeight / 2) - (lineHeight / 2);
        }
        else if(eAlignMask == CCImage::kAlignBottom ||
                eAlignMask == CCImage::kAlignBottomRight ||
                eAlignMask == CCImage::kAlignBottomLeft)
        {
            result = maxLineHeight - lineHeight;
        }
        // In all other cases (top alignment, most likely), return 0;

        if(result < 0)
        {
            result = 0;
        }
        return result;
    }

	bool getBitmap(const char *text, int nWidth, int nHeight, CCImage::ETextAlign eAlignMask, const char * pFontName, float fontSize) {
		const char* pText = text;
        int pxSize = (int)fontSize;

        TTF_Font *face = TTF_OpenFont(pFontName, pxSize);
        if(!face)
        {
            return false;
        }

        divideString(face, text, nWidth, nHeight);

        //compute the final line width
        iMaxLineWidth = MAX(iMaxLineWidth, nWidth);

        iMaxLineHeight = pxSize;
        iMaxLineHeight *= vLines.size();

        //compute the final line height
        iMaxLineHeight = MAX(iMaxLineHeight, nHeight);

        uint bitmapSize = iMaxLineWidth * iMaxLineHeight * 4;
        m_pData = new unsigned char[bitmapSize];
        memset(m_pData, 0, bitmapSize);

        if(!strlen(text))
        {
            return true;
        }

        // XXX: Can this be optimized by inserting newlines into the string and
        // making a single TTF_RenderText_Solid call? Could conceivably just
        // pass back SDL's buffer then, though would need additional logic to
        // call SDL_FreeSurface appropriately.

        // Y offset for vertical alignment remains constant throughout, as it
        // is the entire block of text that must be vertically aligned.
        int yOffset = computeLineStartY(eAlignMask, vLines.size() * pxSize, iMaxLineHeight);

        for (size_t l = 0; l < vLines.size(); l++) {
            pText = vLines[l].sLineStr.c_str();
            if(!strlen(pText))
            {
                continue;
            }

            SDL_Color color = { 0xff, 0xff, 0xff, 0xff };
            SDL_Surface *tSurf = TTF_RenderText_Solid(face, pText, color);
            if(!tSurf)
            {
                TTF_CloseFont(face);
                return false;
            }

            // The lock/unlock pair is required since Emscripten's SDL
            // implementation copies pixel data from its off-screen canvas to
            // the pixels array in the unlock operation. Without this, we would
            // be reading uninitialized memory.
            SDL_LockSurface(tSurf);
            SDL_UnlockSurface(tSurf);

            // We treat pixels as 32-bit words, since both source and target
            // are rendered as such.
            int *pixels = (int*)tSurf->pixels;
            int *out = (int*)m_pData;

            // Compute offset to produce horizontal alignment.
            int xOffset = computeLineStart(eAlignMask, tSurf->w, iMaxLineWidth);

            // (i, j) should be treated as (x, y) coordinates in the source
            // bitmap. This loop maps those locations to the target bitmap.
            // Need to ensure that those values do not exceed the allocated
            // memory.
            int minWidth = MIN(tSurf->w, iMaxLineWidth);
            for(int j = 0; j < tSurf->h && (j + l * pxSize) < iMaxLineHeight; ++j)
            {
                for(int i = 0; i < minWidth; ++i)
                {
                    int sourceOffset = j * tSurf->w + i;
                    int targetOffset = (l * pxSize + j + yOffset) * iMaxLineWidth + i + xOffset;

                    // HTML5 canvas is non-pre-alpha-multiplied, so alpha-multiply here.
                    unsigned char *p = (unsigned char*) &pixels[sourceOffset];
                    out[targetOffset] = CC_RGB_PREMULTIPLY_ALPHA( p[0], p[1], p[2], p[3] );
                }
            }
            SDL_FreeSurface(tSurf);
        }

        // clear all lines
        vLines.clear();

        TTF_CloseFont(face);

        return true;
    }

public:
	unsigned char *m_pData;
	int libError;
	vector<TextLine> vLines;
	int iInterval;
	int iMaxLineWidth;
	int iMaxLineHeight;
};

static BitmapDC& sharedBitmapDC()
{
    static BitmapDC s_BmpDC;
    return s_BmpDC;
}

bool CCImage::initWithString(
                             const char *    pText,
                             int             nWidth/* = 0*/,
                             int             nHeight/* = 0*/,
                             ETextAlign      eAlignMask/* = kAlignCenter*/,
                             const char *    pFontName/* = nil*/,
                             int             nSize/* = 0*/)
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! pText);
        BitmapDC &dc = sharedBitmapDC();

		std::string fullFontName = pFontName;
    	std::string lowerCasePath = fullFontName;
    	std::transform(lowerCasePath.begin(), lowerCasePath.end(), lowerCasePath.begin(), ::tolower);

        CC_BREAK_IF(! dc.getBitmap(pText, nWidth, nHeight, eAlignMask, fullFontName.c_str(), nSize));
        
        // assign the dc.m_pData to m_pData in order to save time
        m_pData = dc.m_pData;
        CC_BREAK_IF(! m_pData);
        
        m_nWidth = (short)dc.iMaxLineWidth;
        m_nHeight = (short)dc.iMaxLineHeight;
        m_bHasAlpha = true;
        m_bPreMulti = true;
        m_nBitsPerComponent = 8;
        
        bRet = true;
        
        dc.reset();
        
    } while (0);
    
    return bRet;
}

NS_CC_END

