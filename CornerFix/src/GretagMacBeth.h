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
 * GretagMacBeth.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 12 Jan 2008 : 0.9.2.0
 * 19 Sept 2009 : 1.3.0.0
 * 22 Oct 2009 : 1.3.0.2
 *
 */

#pragma once

#include "dng_negative.h"
#include "dng_1d_function.h"
#include "dng_xy_coord.h"
#include "dng_1d_table.h"
#include "dng_bottlenecks.h"
#include "dng_color_space.h"
#include "dng_color_spec.h"
#include "dng_filter_task.h"
#include "dng_host.h"
#include "dng_image.h"
#include "dng_negative.h"
#include "dng_resample.h"
#include "dng_utils.h"



#define NUMGretagPatches 24 //25 with surround.....


class GretagMacBeth
{
public:
	GretagMacBeth(const dng_negative &negative);
	~GretagMacBeth(void);
	dng_vector_3 getpatchRGB(int index);

private:
	const dng_negative &fNegative;
	dng_xy_coord fCameraWhite;
	dng_vector_3 patchRGBs[NUMGretagPatches+1];

	void labToXYZ(dng_vector *lab, dng_vector *XYZ);
};
