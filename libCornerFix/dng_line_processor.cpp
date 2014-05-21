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
 * dng_line_processor.cpp
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
 * 20 Dec 2007 : 0.9.1.8
 * 12 Jan 2008 : 0.9.2.0
 * 1 Sept 2009 : 1.1.1.0;
 * 12 Sept 2009 : 1.2.0.0
 * 13 Oct 2009 : 1.3.0.0
 * 9 Nov 2009 : 1.3.0.3
 * 6 October 2010 : 1.4.0.0
 * 23 January 2011 : 1.4.1.0
 * 8 July 2012 : 1.4.2.1
 *
 */
#include "dng_line_processor.h"
#include "dng_string.h"

/*****************************************************************************/

enum localColors {
	color_red = 0,
	color_green1,
	color_green2,
	color_blue
};

dng_line_processor::dng_line_processor( dng_rect *fActiveAreaVal)
:	fActiveArea (fActiveAreaVal)
{
	lut = NULL;
	rlut = NULL;
	intClipLevel = 0x0ffff;
	antiAliasStrength = 0;
	lutSize = 0;
	for (int i = 0; i < kbayerColors; i++) 
	{
		for (uint32 octant = 0; octant < kMaxOctants; octant++) 
		{		
			factorCache[octant][i] = NULL;
		}
		xOffset[i] = 0;
		yOffset[i] = 0;
	}
	fBlack_2D_rows = 0;
	fBlack_2D_cols = 0;
	for (int i = 0; i < kMaxBlackPattern; i++) 
	{
		for (int j = 0; j < kMaxBlackPattern; j++) 
		{			
			fBlackBuffer[i][j] = 0;
		}
	}

}

dng_line_processor::dng_line_processor()
{
	lut = NULL;
	rlut = NULL;
	intClipLevel = 0x07fffffff;
	antiAliasStrength = 0;
	for (int i = 0; i < kbayerColors; i++) 
	{
		for (uint32 octant = 0; octant < kMaxOctants; octant++) 
		{		
			factorCache[octant][i] = NULL;
		}
	}
	fBlack_2D_rows = 0;
	fBlack_2D_cols = 0;
	for (int i = 0; i < kMaxBlackPattern; i++) 
	{
		for (int j = 0; j < kMaxBlackPattern; j++) 
		{			
			fBlackBuffer[i][j] = 0;
		}
	}
	fActiveArea = NULL;
}

dng_line_processor::~dng_line_processor( void )
{
	for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
	{
		for (uint32 octant = 0; octant < kMaxOctants; octant++) 
		{	
			if (factorCache[octant][case_p]) delete []factorCache[octant][case_p];
			factorCache[octant][case_p] = NULL;
		}
	}
	if (rlut) delete []rlut;
}

void dng_line_processor::setLUT(uint16 *newLut, uint32 lutSizeVal)
{
	lut = newLut;
	lutSize = lutSizeVal;
	// White level is really for the linearization process; the lut can be bigger......
	if (intClipLevel < lut[lutSize-1]) intClipLevel = lut[lutSize-1];
}

void dng_line_processor::setActiveArea(dng_rect *fActiveAreaVal)
{
	fActiveArea = fActiveAreaVal;
}


void dng_line_processor::buildRlut(dng_string &cameraType)
{
	//		rlut = lut;
	if (lut && intClipLevel > 2) {
		rlut = new uint16[intClipLevel+1];
	}
	int32 rlutIndex = 0;
	if (!(cameraType.StartsWith("M8") || cameraType.StartsWith("M9")) || (lutSize != 256))
	{
		real32 ucompSq = 0;
		real32 prevucompSq = 0;
		real32 k = 0;
		real32 c = 0;
		for (uint32 i = 1; i < lutSize; i++)
		{
			ucompSq = pow((real32) lut[i], 0.5F);
			if (lut[i-1] != lut[i])
			{
				k =  (-1.0F)/(prevucompSq - ucompSq);
				c = ((real32) i) - k*ucompSq;
			}
			else
			{
				k = 0.0;
				c = lut[i];
			}
			prevucompSq = ucompSq;
			
			//			dng_local_printf::printf ("%d, %d, %d, %d\n", i-1, i, lut[i-1], lut[i]);
			for (rlutIndex = lut[i-1]; rlutIndex < lut[i]; rlutIndex++)
			{
				rlut[rlutIndex] = (uint16) (k * pow((real32) rlutIndex,0.5F) + c);
			}
			//			dng_local_printf::printf ("%ld, %ld, %ld, %ld\n", i, lut[i], rlutIndex-1, rlut[rlutIndex-1]);
		}
		for (; rlutIndex <= intClipLevel; rlutIndex++)
		{
			rlut[rlutIndex] = (uint16) (k * pow((real32) rlutIndex,0.5F) + c);
			//			dng_local_printf::printf ("%ld, %ld\n",rlutIndex, rlut[rlutIndex]);
		}
	}
	else {
		// M8 or M9
		for (rlutIndex = 0; rlutIndex <= intClipLevel; rlutIndex++)
		{
			rlut[rlutIndex] = (uint16) (2.0 * pow((real32) rlutIndex,0.5F)+0.5);
			if (rlut[rlutIndex] > 255) rlut[rlutIndex] = 255;
		}
	}
}

void dng_line_processor::setClipLevel(int32 intClipLevelVal)
{
	intClipLevel = intClipLevelVal;
	// White level is really for the linearization process; the lut can be bigger......
	if (lut) {
		if (intClipLevel < lut[lutSize-1]) intClipLevel = lut[lutSize-1];
	}
	
}


void dng_line_processor::setAntiAlias(int antiAlias)
{
	antiAliasStrength = antiAlias;
}

// Note must call setActive area before setOffsets
void dng_line_processor::setOffsets(uint32 color, int32 x, int32 y)
{
	xOffset[color] = x + (int32)(fActiveArea->W()>>1);
	yOffset[color] = y + (int32)(fActiveArea->H()>>1);
}


void dng_line_processor::buildFactorCache(uint32 octant, bool isSinglePlaneCFA, uint32 plane, dng_lmfit_functions *mainFunction, dng_lmfit_functions *adjacentFunctionA, dng_lmfit_functions *adjacentFunctionB, uint32 cacheLength)
{
	real32 radius, sM, sA, sB;
	int32 mainRadius[kbayerColors];
    
    if (!isSinglePlaneCFA) {
        uint32_t color = 0;
        switch (plane) {
            case 0:
                color = color_red;
                break;
            case 1:
                color = color_green1;
                break;
            case 2:
                color = color_blue;
                break;
            default:
                break;
        }
        for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
        {
            xOffset[case_p] = xOffset[color];
            yOffset[case_p] = yOffset[color];
        }
    }
	
	// Sort out the inner/outer transition points between the vertical/horizontal and diagonal equations
	for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
	{
		// We don't support more than 1 million pixels a side(!)
		mainRadius[case_p] = 999999;
		if (mainFunction != NULL) {
			switch (octant) {
				case kLeftHorizonalOctant:
					mainRadius[case_p] = ((int32) fActiveArea->W()>>1) + mainFunction[case_p].getCenterX();
					break;
				case kBottomVerticalOctant:
					mainRadius[case_p] = ((int32) fActiveArea->H()>>1) - mainFunction[case_p].getCenterY();
					break;
				case kTopVerticalOctant:
					mainRadius[case_p] = ((int32) fActiveArea->H()>>1) + mainFunction[case_p].getCenterY();
					break;
				case kRightHorizonalOctant:
					mainRadius[case_p] = ((int32) fActiveArea->W()>>1) - mainFunction[case_p].getCenterX();
					break;
				case kBottomLeftDiagonalOctant:	
				case kTopRightDiagonalOctant:
				case kBottomRightDiagonalOctant:
				case kTopLeftDiagonalOctant:
					break;
				default:
				break;
			}	
		}
	}

	// CornerFix uses the convention of "negative" radius values; not really useful multi-segment, 
	// but preserved because it is useful in single segment. This sets up the correct signs for
	// those values
	switch (octant) {
		case kBottomVerticalOctant:
			sA = -1.0F;
			sB = 1.0F;
			sM = 1.0F;
			break;
		case kLeftHorizonalOctant:
		case kTopLeftDiagonalOctant:
		case kBottomLeftDiagonalOctant:	
		default:
			sA = -1.0F;
			sB = -1.0F;
			sM = -1.0F;
			break;
		case kTopVerticalOctant:
			sA = -1.0F;
			sB = 1.0F;
			sM = 1.0F;
			break;
		case kTopRightDiagonalOctant:
		case kRightHorizonalOctant:
		case kBottomRightDiagonalOctant:
			sA = 1.0F;
			sB = 1.0F;
			sM = 1.0F;
			break;
	}
	
	// Interpolation between the inner and outer segments, and get the required memory
	real32 cfoc[kbayerColors];
	for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
	{
		factorCache[octant][case_p] = new real32[cacheLength];
		cfoc[case_p] = 0.5;
		if ((mainFunction != NULL) && (adjacentFunctionA != NULL) && (adjacentFunctionB != NULL)) {
			radius = (real32) mainRadius[case_p];
			cfoc[case_p] = ((real32) mainFunction[case_p].evalTweaked(radius*sM))/
			((real32) adjacentFunctionA[case_p].evalTweaked(radius*sA) + 
			 (real32) adjacentFunctionB[case_p].evalTweaked(radius*sB));
		}
	}
	
	// And finally we build the factor cache
	for (uint32 r = 0; r < cacheLength; r++)
	{
		radius = (real32) r;
		uint32 color;
			
//		if (octant == kTopLeftDiagonalOctant) dng_local_printf::printf ("\n%ld ", r);
		for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
		{	
            // Single plane means bayer.....
			if (isSinglePlaneCFA) {
				color = case_p;
			}
			else {
				switch (plane) {
					case 0:
						color = color_red;
						break;
					case 1:
						color = color_green1;
						break;
					case 2:
						color = color_blue;
						break;
					default:
						break;
				}
			}
			
			factorCache[octant][case_p][r] = ((mainFunction != NULL) && ((r <= ((uint32) mainRadius[color])) || (adjacentFunctionA == NULL) || (adjacentFunctionB == NULL))) ?
												(real32) mainFunction[color].evalTweaked(radius*sM) : 
												((real32) adjacentFunctionA[color].evalTweaked(radius*sA) + (real32) adjacentFunctionB[color].evalTweaked(radius*sB))*cfoc[color];
			
//			if (octant == kTopLeftDiagonalOctant) {
//				dng_local_printf::printf ("%f %f ", (float) octant, factorCache[octant][case_p][r]);
//			}
		}
//		if (octant == kTopLeftDiagonalOctant) dng_local_printf::printf ("\n"); 
	}
}

void dng_line_processor::setPatchRGB(uint16 *val) {
	patchRGB = val;
}

void dng_line_processor::setLinearisationInfo(dng_host &host, const dng_image &srcImage, const dng_linearization_info &info, int plane)
{
	fBlack_2D_rows = info.fBlackLevelRepeatRows;
	fBlack_2D_cols = info.fBlackLevelRepeatCols;
	
	for (uint32 i = 0; i < fBlack_2D_rows; i++) {
		for (uint32 j = 0; j < fBlack_2D_cols; j++) {		
			fBlackBuffer[i][j] = (int32) info.fBlackLevel [i] [j] [plane];
		}
	}
	
	if ((fBlack_2D_rows > 0) || (fBlack_2D_cols > 0) || info.fBlackDeltaV.Get() || info.fBlackDeltaH.Get()) {
		// Here we will need black level processing
		blackDeltaH.Reset (host.Allocate (srcImage.Width() * sizeof (real64)));
		for (uint32  j = 0; j < srcImage.Width(); j++) {
			blackDeltaH->Buffer_real64 ()[j] = 0.0;
		}
		blackDeltaV.Reset (host.Allocate (srcImage.Height() * sizeof (real64)));
		for (uint32  j = 0; j < srcImage.Height(); j++) {
			blackDeltaV->Buffer_real64 ()[j] = 0.0;
		}
		if (fBlack_2D_cols < 1) {
			fBlack_2D_rows = 1;
			fBlack_2D_cols = 1;
			for (uint32 i = 0; i < fBlack_2D_rows; i++) {
				for (uint32 j = 0; j < fBlack_2D_cols; j++) {		
					fBlackBuffer[i][j] = 0;
				}
			}		
		}		
	}

	// Now the BlackDeltas
	if (info.fBlackDeltaH.Get()) {
		for (uint32  j = 0; j < info.fActiveArea.W(); j++) {
			blackDeltaH->Buffer_real64 ()[j+info.fActiveArea.l] = info.fBlackDeltaH->Buffer_real64 () [j];
		}
	}
	if (info.fBlackDeltaV.Get()) {
		for (uint32  k = 0; k < info.fActiveArea.H(); k++) {
			blackDeltaV->Buffer_real64 ()[k+info.fActiveArea.t] = info.fBlackDeltaV->Buffer_real64 () [k];
		}
	}
}


