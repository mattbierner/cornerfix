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
 * CornerFixRegistry.h
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

#pragma once

using namespace System;
using namespace Microsoft::Win32;
using namespace System::Security::Permissions;

// Magic to get the security right..... 
[assembly:RegistryPermissionAttribute(SecurityAction::RequestMinimum,
ViewAndModify="HKEY_CURRENT_USER")];


ref class CornerFixRegistry
{
public:
	CornerFixRegistry(void);
	~CornerFixRegistry(void);

	void set(String^ name, Object^ value);
	String^ get(String^ name, String^ defaultVal);
	int get(String^ name, int defaultVal);
	bool get(String^ name, bool defaultVal);

private:
	RegistryKey^ rk;
};
