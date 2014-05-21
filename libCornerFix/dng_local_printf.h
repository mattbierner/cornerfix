/* ======================================
 * CornerFix - Fixing the M8 Cyan Corners
 * ======================================
 *
 * Project Info:  http://sourceforge.net/projects/cornerfix
 * Project Lead:  Sandy McGuffog (sandy.cornerfix@gmail.com);
 *
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * ---------------
 * dng_local_printf.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 01 Jun 2007 : Original version;
 * 1 Sept 2007 : 0.9.1.0 Mac version
 * 4 Sept 2007 : 0.9.1.1 Mac version
 *
 */

/*
 * This class providesa translation between:
 * a) printf's in the original dng SDK code
 * b) dng data types to .NET data types
 * c) an interface between managed and unmanaged code
 * It's (c) that actually causes trouble.....
 * This is a singleton design pattern.
 * In theory, this more eaily be implemented with either
 * templates or variable length arguments - unfortunately,
 * VC2005's implementation of both is broken.
 */

#pragma once
#include "dng_types.h"

#define MULTITHREAD 1

enum
	{
	progressNone				= 0,
	progressReading,
	progressDevignette,
	progressLinearization,
	progressIlluminant,
	progressAnalysis,
	progressInterpolate,
	progressRender,
	progressWriteTIFF,
	progressThumbnail,
	progressWriteDNG,
};

#define BUF_LENGTH 5000

#if qWinOS
ref class dng_local_printf
#else
class dng_local_printf
#endif
{
public: 
#if qWinOS
    static dng_local_printf^ Instance();
	static void setBox(System::Windows::Forms::RichTextBox ^b);
	static void setWorker(System::ComponentModel::BackgroundWorker ^w);
	static void updateText(System::String ^text);
#else
    static dng_local_printf* Instance();	
	static void setBox(void * controller);
	static void setWorker(void * worker);
#endif
	static void addText(char * t);
	static void workerProgress(int val);
	static void printf(const char * format);
	static void printf(const char * format, const char *s);
	static void printf(const char * format, const char *s, const char *s2, const char *s3, const char *s4);
	static void printf(const char * format, uint32 s);
	static void printf(const char * format, uint8 s);
	static void printf(const char * format, int32 s);
#if qWinOS
	static void printf(const char * format, int s);
	static void printf(const char * format, unsigned s);
#endif
	static void printf(const char * format, char s);
	static void printf(const char * format, uint32 s, real64 s2);
	static void printf(const char * format, real64 s);
	static void printf(const char * format, const char *s, int s2);
	static void printf(const char * format, const char *s, unsigned s2);
	static void printf(const char * format, const char *s, real64 s2);
	static void printf(const char * format, real64 s, real64 s2);
	static void printf(const char * format, real64 s, real64 s2, real64 s3, real64 s4);
	static void printf(const char * format, real64 s, real64 s2, real64 s3, real64 s4, real64 s5, real64 s6);
	static void printf(const char * format, real64 s, real64 s2, real64 s3);
	static void printf(const char * format, unsigned s, unsigned s2, unsigned s3, unsigned s4);
	static void printf(const char * format, unsigned s, unsigned s2);
	static void printf(const char * format, int s, int s2);
	static void printf(const char * format, int s, int s2, int s3, int s4, int s5, int s6);
	static void printf(const char * format, int s, int s2, int s3, int s4);
	static void printf(const char * format, int s, real64 s2, real64 s3, real64 s4, real64 s5);
	static void printf(const char * format, const char *s, unsigned s2, unsigned s3);
	
protected:

	dng_local_printf();

private:
#if qWinOS
    static dng_local_printf^ _instance;
	static System::Windows::Forms::RichTextBox^ box;
	static System::ComponentModel::BackgroundWorker ^worker;
	delegate void updateTextDelegate(System::String ^text);
	static updateTextDelegate^ myDelegate;
#else
	static void * panelController;
	static void * workerController;
	static dng_local_printf* _instance;
#endif

};

