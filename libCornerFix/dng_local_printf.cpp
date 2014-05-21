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
 * dng_local_printf.cpp
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
 * 29 Nov 2007 : 0.9.1.7
 * 19 Sept 2009 : 1.3.0.0;
 * 4 July 2011 : 1.4.2.0
 *
 */

/*
 * This class provides a translation between:
 * a) printf's in the original dng SDK code
 * b) dng data types to .NET data types
 * c) an interface between managed and unmanaged code
 * It's (c) that actually causes trouble.....
 * This is a singleton design pattern.
 * In theory, this more eaily be implemented with either
 * templates or variable length arguments - unfortunately,
 * VC2005's implementation of both is broken.
 */

#include "dng_flags.h"
#if qWinOS
#include "StdAfx.h"
#else
#import <Cocoa/Cocoa.h>
//#import "PanelController.h"
#endif
#include "dng_local_printf.h"
#include <stdio.h>

dng_local_printf::dng_local_printf() {

}


#if qWinOS
dng_local_printf^ dng_local_printf::Instance() {
    if (!_instance) {
        _instance = gcnew dng_local_printf;
		box = nullptr;
    }
    return _instance;
}

void dng_local_printf::setBox(System::Windows::Forms::RichTextBox^ b)
{

	box = b;
	myDelegate = gcnew updateTextDelegate( &dng_local_printf::updateText );

}


void dng_local_printf::setWorker(System::ComponentModel::BackgroundWorker ^w)
{

	worker = w;

}

void dng_local_printf::workerProgress(int val)
{

	worker->ReportProgress(val);

}


void dng_local_printf::updateText(System::String ^text)
{
  // Set the textbox text.
  box->AppendText(text);
  if (text->Contains("\n")) {
	box->ScrollToCaret();
  }
  
}




void dng_local_printf::addText(char * t) {
	if (box) 
	{
		System::String ^text= gcnew System::String(t);
		if (box->InvokeRequired)
		{
			array<Object^>^myStringArray = {text};
			box->Invoke( myDelegate, myStringArray );
  		}
		else {
			updateText(text);
		}

	}
	else
	{
		printf(t);
	}
}
#else

void * dng_local_printf::panelController;
void * dng_local_printf::workerController;
dng_local_printf* dng_local_printf::_instance;

dng_local_printf* dng_local_printf::Instance() {
    if (!_instance) {
        _instance = new dng_local_printf;
		panelController = nil;
		workerController = nil;
    }
    return _instance;
}


void dng_local_printf::setBox(void * controller)
{

	panelController = controller;

}

void dng_local_printf::addText(char * t) {

	if (!panelController) {
		fprintf(stderr, "%s", t);
		}
	else {
#if defined(MULTITHREAD)
		// A piece of magic to get this to work when we're multi-threaded....... 
		[((id) panelController) performSelectorOnMainThread:@selector(addTextToPanel:) withObject:[[NSString alloc]
			initWithCString: t encoding: NSUTF8StringEncoding] waitUntilDone:NO];
#else
		[((id) panelController) addTextToPanel:[[NSString alloc] initWithCString: t encoding: NSUTF8StringEncoding]];
#endif
	} 

}

void dng_local_printf::setWorker(void * worker)
{
	
	workerController = worker;
	
}


void dng_local_printf::workerProgress(int val)
{
	if (workerController) {
		[((id) workerController) performSelectorOnMainThread:@selector(updateProgress:) withObject:[NSNumber numberWithInt:val] waitUntilDone:NO];
	}
		

}
#endif


void dng_local_printf::printf(const char * format, const char *s)
{
	char buf[BUF_LENGTH];

#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);

}

void dng_local_printf::printf(const char * format, const char *s, const char *s2, const char *s3, const char *s4)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4);
#else
	sprintf(buf, format, s, s2, s3, s4);
#endif
	addText(buf);
}


void dng_local_printf::printf(const char * format)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format);
#else
// we really have to do this.....
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
	sprintf(buf, format);
#pragma GCC diagnostic pop
#endif
	addText(buf);
}

void dng_local_printf::printf(const char * format, uint32 s)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, uint8 s)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, int32 s)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);
	
}

#if qWinOS
void dng_local_printf::printf(const char * format, int s)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, unsigned s)
{
	char buf[BUF_LENGTH];
	sprintf_s(buf, BUF_LENGTH, format, s);
	addText(buf);
	
}

#endif

void dng_local_printf::printf(const char * format, char s)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);
	
}


void dng_local_printf::printf(const char * format, uint32 s, real64 s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, real64 s)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s);
#else
	sprintf(buf, format, s);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, const char *s, int s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, const char *s, unsigned s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, const char *s, real64 s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, real64 s, real64 s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, real64 s, real64 s2, real64 s3, real64 s4)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4);
#else
	sprintf(buf, format, s, s2, s3, s4);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, real64 s, real64 s2, real64 s3, real64 s4, real64 s5, real64 s6)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4, s5, s6);
#else
	sprintf(buf, format, s, s2, s3, s4, s5, s6);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, real64 s, real64 s2, real64 s3)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3);
#else
	sprintf(buf, format, s, s2, s3);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, unsigned s, unsigned s2, unsigned s3, unsigned s4)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4);
#else
	sprintf(buf, format, s, s2, s3, s4);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, unsigned s, unsigned s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, int s, int s2)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2);
#else
	sprintf(buf, format, s, s2);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, int s, int s2, int s3, int s4, int s5, int s6)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4, s5, s6);
#else
	sprintf(buf, format, s, s2, s3, s4, s5, s6);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, int s, int s2, int s3, int s4)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4);
#else
	sprintf(buf, format, s, s2, s3, s4);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, int s, real64 s2, real64 s3, real64 s4, real64 s5)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3, s4, s5);
#else
	sprintf(buf, format, s, s2, s3, s4, s5);
#endif
	addText(buf);
	
}

void dng_local_printf::printf(const char * format, const char *s, unsigned s2, unsigned s3)
{
	char buf[BUF_LENGTH];
#if qWinOS
	sprintf_s(buf, BUF_LENGTH, format, s, s2, s3);
#else
	sprintf(buf, format, s, s2, s3);
#endif
	addText(buf);
	
}
