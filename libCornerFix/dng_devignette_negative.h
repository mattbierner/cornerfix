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
 * dng_devignette_negative.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 1 Jul 2007 : 0.9.1.0
 * 9 Nov 2009 : 1.3.0.3
 *
 */
#pragma once
#include "dng_negative.h"
#include "dng_lmfit_functions.h"
#include "dng_host.h"
#include "M8_devignette.h"

class dng_devignette_negative
{
public:
	dng_devignette_negative(void);
	~dng_devignette_negative(void);
	virtual void SetChromaStrength(int strength);
	virtual void SetLumaStrength(int strength);
void doDeVignette (dng_host &host, const dng_linearization_info &info, const dng_image &srcImage, 
				   dng_image &dstImage, M8_devignette *vDescriptor, int antiAliasStrength,
				   dng_string &cameraType);
private:
	int chromaStrength;
	int lumaStrength;

};
