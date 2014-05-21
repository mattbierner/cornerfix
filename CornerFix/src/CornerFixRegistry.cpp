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
 * CornerFixRegistry.cpp
 * ---------------
 * (C) Copyright 2007-2009, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 21 Jun 2007 : 0.9.0.6
 *
 */

/*
 * Registry interface for CornerFix
 */

// There is some heavy try/catching going on here because the Windows
// registry calls are not very reliable in what they return between
// versions of Windows - fall back to defaults if something unexpected happens

#include "CornerFixRegistry.h"

CornerFixRegistry::CornerFixRegistry(void)
{
	try
	{
		rk = Registry::CurrentUser->CreateSubKey("Software\\CornerFix");
	}
	catch (...)
	{
		try
		{
			rk = Registry::CurrentUser->CreateSubKey("\\Software\\CornerFix");
		}
		catch (...)
		{
			rk = nullptr;
		}
	}
}

CornerFixRegistry::~CornerFixRegistry(void)
{
}

void CornerFixRegistry::set(String^ name, Object^ value)
{
	if (rk) rk->SetValue(name, value);
}

String^ CornerFixRegistry::get(String^ name, String^ defaultVal)
{
	try 
	{
		if (rk) return dynamic_cast<String^>(rk->GetValue(name, defaultVal));
		else return defaultVal;
	}
	catch (...)
	{
		return defaultVal;
	}
}

int CornerFixRegistry::get(String^ name, int defaultVal)
{
	try 
	{
		if (rk) return (int) (rk->GetValue(name, defaultVal));
		else return defaultVal;
	}
	catch (...)
	{
		return defaultVal;
	}
}

bool CornerFixRegistry::get(String^ name, bool defaultVal)
{
	try 
	{
		if (rk) return Boolean::Parse((String^) rk->GetValue(name, defaultVal.ToString()));
		else return defaultVal;
	}
	catch (...)
	{
		return defaultVal;
	}
}
