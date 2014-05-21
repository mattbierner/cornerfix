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
 * dng_string_win.h
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
 * 21 Sept 2007 : 0.9.1.4
 *
 */

/*
 * Converts from .NET String to char, or NSString to char on the Mac *
 */

#if qWinOS
#include "StdAfx.h"
#else
#include <Cocoa/Cocoa.h>
#endif
#include "dng_string_win.h"

dng_string_win::dng_string_win(void)
{
}

dng_string_win::~dng_string_win(void)
{
}


#if qWinOS
void dng_string_win::setFromString(String ^s)
{
	if (s == "") {
		this->Clear();
	}
	else {
		const char* Str = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		// Do what you want with FileStr
		this->Set(Str);
		Marshal::FreeHGlobal(IntPtr((void*)Str));
	}

}
#else
void dng_string_win::setFromString(NSString *s)
{
	if ([s length] == 0) {
		this->Clear();
	}
	else {
		//This does a POSIX friendly conversion for UNICODE strings
		this->Set([[NSFileManager defaultManager] fileSystemRepresentationWithPath:s]);
	}

}
#endif