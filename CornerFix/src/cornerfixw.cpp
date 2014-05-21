/* ======================================
 * CornerFix - Fixing the M8 Cyan Corners
 * ======================================
 *
 * Project Info:  http://sourceforge.net/projects/cornerfix
 * Project Lead:  Sandy McGuffog (sandy.cornerfix@gmail.com);
 *
 * (C) Copyright 2007-2009, by Sandy McGuffog and Contributors.
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
 * cornerfixw.cpp
 * ---------------
 * (C) Copyright 2007-2009, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 14 July 2008 : 1.1.0.1
 *
 */

#include "stdafx.h"
#include "Form1.h"

using namespace cornerfixw;
#include <tchar.h>
#using <system.dll>
#using <mscorlib.dll>

using namespace System;
using namespace System::Diagnostics;

// Note - to get this to compile cleanly, 
// need to set #pragma unmanaged in the xmp_const file of the XMP SDK
// else the lib has a struct that the /clr option doesn't like

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
//	 Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	try {
		Application::Run(gcnew Form1());
	}
	catch(Exception^ ex) {
		String ^sSource = gcnew String("CornerFix For Windows");
		String ^sLog = gcnew String("CornerFix");

		try {
			if(!EventLog::SourceExists(sSource)) EventLog::CreateEventSource(sSource,sLog);

			EventLog::WriteEntry(sSource,ex->ToString());
			EventLog::WriteEntry(sSource, ex->ToString(), EventLogEntryType::Error, 234);
		}
		catch (...) {
			throw (ex);
		}
	}
	return 0;
}
