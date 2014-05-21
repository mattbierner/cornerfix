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
 * dng_line_processor.h
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
 * 15 Sept 2007 : 0.9.1.3
 * 12 Jan 2008 : 0.9.2.0
 * 12 Sept 2009 : 1.2.0.0
 * 13 Oct 2009 : 1.3.0.0
 * 21 Oct 2009 : 1.3.0.1
 * 22 Oct 2009 : 1.3.0.2
 * 9 Nov 2009 : 1.3.0.3
 * 6 October 2010 : 1.4.0.0
 * 23 January 2011 : 1.4.1.0
 * 8 July 2012 : 1.4.2.1
 *
 */

#include "dng_linearization_info.h"
#include "dng_lmfit_functions.h"
#include "GretagMacBeth.h"
#if qMacOS
//#include <Cocoa/Cocoa.h>
#endif

enum
{

// constants.	
kdimensions = 3,
kbayerColors = 4

};

// This has some template functions to ease pixel type to pixel type
// conversions.....

// The class as a whole processes a single line (contiguous set of pixels) in a single row.
// Its built such that it interfaces to a tile processor.....

class dng_line_processor 
{
	
	
private:
    uint16 *lut;
    uint16 *rlut;
	dng_rect *fActiveArea;
	int32 intClipLevel;
	int antiAliasStrength;
	uint32 lutSize;
	real32 *factorCache[kMaxOctants][kbayerColors];
	uint16 *patchRGB;
	
	// Black level storage
	uint32 fBlack_2D_rows;
	uint32 fBlack_2D_cols;
	int32 fBlackBuffer[kMaxBlackPattern][kMaxBlackPattern];
	int32 xOffset[kbayerColors];
	int32 yOffset[kbayerColors];
	AutoPtr<dng_memory_block> blackDeltaH;
	AutoPtr<dng_memory_block> blackDeltaV;	
	
public:
    dng_line_processor( dng_rect *fActiveAreaVal);
    dng_line_processor();
	~dng_line_processor( void );
	void setLUT(uint16 *newLut, uint32 lutSizeVal);
	void setActiveArea(dng_rect *fActiveAreaVal);
	void buildRlut(dng_string &cameraType);
	void setClipLevel(int32 intClipLevelVal);
	void setAntiAlias(int antiAlias);
	void buildFactorCache(uint32 octant, bool isSinglePlaneCFA, uint32 plane, dng_lmfit_functions *mainFunction, dng_lmfit_functions *adjacentFunctionA, dng_lmfit_functions *adjacentFunctionB, uint32 cacheLength);
	void setPatchRGB(uint16 *val);
	void setLinearisationInfo(dng_host &host, const dng_image &srcImage, const dng_linearization_info &info, int plane);
	// Note must call setActive area before setOffsets
	void setOffsets(uint32 color, int32 x, int32 y);
	template <class sT, class dT>	
	void processLine(const void *sPtr, void *dPtr, uint32 srcRow, uint32 srcCol, int32 sStep, int32 dStep, uint32 count, int32 top, int32 left)
	{
		sT *srcPtr = (sT *) sPtr;
		dT *dstPtr = (dT *) dPtr;
#ifdef DMR_HACK
#if qMacOS
		//	BOOL levelCompress = [[NSUserDefaults standardUserDefaults] boolForKey:@"levelCompression"];
#endif
#endif
		
		uint32 b2_count = fBlack_2D_cols;
		uint32 b2_phase = 0;
		
		if (b2_count >0) {
			b2_phase = srcCol % b2_count;
		}
		
		real32 slope = ((real32) fActiveArea->H())/((real32) fActiveArea->W());
		int32 offRowInt = ((int32) srcRow) - (((int32) fActiveArea->H()>>1) + top);
		real32 offRow = (real32) offRowInt;
		
		for (uint32 j = 0; j < count; j++)
		{
			int32 val = 0;
			real32 rval = 0.0F;
			real32 rvalExtended = 0.0;
			real32 deAlias = 0.0;
			real32 blackOffset = 0.0;
			real32 blackScale = 1.0;
			uint32 dstCol = srcCol + j;
			uint32 case_p = (dstCol & 0x1) + ((srcRow & 0x1) << 1);

			int32 offColInt = ((int32) dstCol) - (((int32) fActiveArea->W()>>1) + left);			
			real32 offCol = (real32) offColInt;	
			
			real32 factor = 1.0;
			uint32 radius = (uint32) sqrt(pow((real32) (((int32) srcRow) - top - yOffset[case_p]), 2.0F)
										  + pow((real32) (((int32) dstCol) - left - xOffset[case_p]), 2.0F));
			uint32 octant = 0;
			if (offRowInt >= 0) {
				octant |= 4;
				if (offColInt < 0) {
					octant ^= 3;
				}
			}
			else {
				if (offColInt >= 0) {
					octant ^= 3;
					}
			}
			if ((fabs(offCol)*slope) < fabs(offRow)) {
				octant ^= 1;
			}
			
#if _DEBUG
//			bool lineDump = false;
//            if ((srcRow < 2) && (dstCol > 13) && (dstCol < 16)) {
//                lineDump = true;
//            }
			// Square pattern
//			lineDump |= ((srcRow & 0x1ff) == 3) && ((dstCol & 0x1ff) == 3);
			// Radius
//			lineDump |= (radius > 2980) && (radius < 3010) && (srcRow < 4000);
			// Verticals on the edges
// 			lineDump |= (dstCol >= (fActiveArea->W()-2)) || (dstCol <= 1);
			// Vertical on the center
//			lineDump |= (dstCol+1 == (fActiveArea->W()>>1)) || (dstCol == (fActiveArea->W()>>1));
			// Vertical on the 25%
//			lineDump |= (dstCol+1 == (fActiveArea->W()>>2)) || (dstCol == (fActiveArea->W()>>2));
			// Vertical on the 75%
//			lineDump |= (dstCol+1 == ((fActiveArea->W()>>2)*3)) || (dstCol == ((fActiveArea->W()>>2)*3));
			
			// Decimate by 16
//			lineDump &= ((srcRow & 0x2c) == 0);
#endif
			
			if (factorCache[octant][case_p]) {
				
				bool above = (fabs(offCol)*slope) < fabs(offRow);
				
				real32 adjustVal = 0.0;
				if (above) {
					adjustVal = offRowInt == 0 ? 1.0F : (real32) fabs(offCol)/(fabs(offRow)/slope);
				}
				else {
					adjustVal = offColInt == 0 ? 1.0F : (real32) fabs(offRow)/(fabs(offCol)*slope);
				}
				adjustVal = ((octant & 0x1) == 0) ? 1.0F - adjustVal : adjustVal;
				
#if _DEBUG				
//				if (lineDump) printf("\n%d, %d, %f, %d, %d", (int) offRow, (int) offCol, adjustVal, octant, ((octant+1)&0x7));
#endif		
				factor = factorCache[octant][case_p][radius]*adjustVal + factorCache[((octant+1)&0x7)][case_p][radius]*(1.0F-adjustVal);
#if _DEBUG				
//				if (lineDump) printf(" :: %d, %f, %f, %f", radius, factorCache[octant][case_p][radius], factorCache[((octant+1)&0x7)][case_p][radius], factor);
#endif				

			}
			
			if (b2_count > 0) {
//				blackOffset = ((real32) (fBlackBuffer[srcRow % fBlack_2D_rows][b2_phase])) + blackDeltaV->Buffer_real64 ()[srcRow] + blackDeltaH->Buffer_real64 ()[dstCol];
				blackOffset = ((real32) (fBlackBuffer[srcRow % fBlack_2D_rows][b2_phase]))+ (real32) blackDeltaV->Buffer_real64 ()[srcRow];
				blackScale = ((real32 ) intClipLevel)/(((real32) intClipLevel) - blackOffset);
			}
			
			if (lut)
			{
				// Here we have a LUT, so first work out anti-alias.......
				if (antiAliasStrength > 0)
				{
					deAlias = (((real32) rand())/((real32) RAND_MAX) - 0.5F)*((real32) antiAliasStrength)/100.0F;			
					
					if ((*srcPtr == 0) || (*srcPtr == (lutSize - 1))) {
						rval = (real32) (lut[(*srcPtr)]);
						rvalExtended = rval;
					}
					else {
						if (deAlias < 0) {
							rval = (real32) (lut[(*srcPtr) - 1]);
							rvalExtended = (real32) lut[(*srcPtr)];
						}
						else {
							rvalExtended = (real32) lut[(*srcPtr)];
							rval = (real32) lut[(*srcPtr)+1];
						}
					}
					
					if (b2_count > 0) {
						rval = (rval - blackOffset) * blackScale;
						rval = rval < 0 ? 0 : rval;
						rvalExtended = (rvalExtended - blackOffset) * blackScale;
						rvalExtended = rvalExtended < 0 ? 0 : rvalExtended;
					}
					
					if (factorCache[octant][case_p])
					{
						rval = rval + deAlias*(rvalExtended - rval)/factor;
					}
					else
					{
						rval = rval + deAlias*(rvalExtended - rval);
					}

					val = (int32) rval;
					if (val > intClipLevel) val = intClipLevel;
				}
				else {
					// Here we have a LUT, but no anti-alias
					rval = (real32) lut[(*srcPtr)];
					if (b2_count > 0) {
						rval = (rval - blackOffset) * blackScale;
						rval = rval < 0 ? 0 : rval;
					}
					if (factorCache[octant][case_p]) rval = rval/factor;
					val = (int32) rval;
					if (val > intClipLevel) val = intClipLevel;
				}
				
				if (rlut)
				{
					// This is an M8 override...........
					//	val = (int32) sqrt(rval*4.0);
					val = rlut[val];
				}
			}
			else
			{
				// Here there is not a LUT........
#if _DEBUG	
//				uint32 oVal = (*srcPtr);
#endif
				rval = (real32) (*srcPtr);
				if (b2_count > 0) {
					rval = (rval - blackOffset) * blackScale;
					rval = rval < 0 ? 0 : rval;
				}
#ifdef DMR_HACK
				// Get to a 14-bit value
				rval = (real32) (val >> 2);
				// This section only for generating M8 test images from DNG files......	
#if qMacOS
				/*
				 if(levelCompress) {
				 rval = (real32) val;
				 // Now do an M8 compression
				 val = (int32) sqrt(rval*4.0);
				 // Then convert back....
				 rval = ((real32) val);
				 rval = (rval*rval)/4;
				 // At this point rval is the level compressed equivalent.....
				 val = (int32) rval;
				 // uncommenting the next line gives the difference
				 val = (((*srcPtr) >> 2) - val);
				 }
				 */
#endif
				// End M8 test image generation
				
#else
				//				min = min > val ? val : min;
				if (factorCache[octant][case_p]) {
					rval = rval/factor;
				}
#endif
				val = (int32) rval;
				if (val > intClipLevel) {
					val = intClipLevel;	
				}
#if _DEBUG	
//			if (lineDump) printf("\n%d, %d, %d, %d, %d, %d, %f", srcRow, dstCol, case_p, oVal, val, radius, factor);
#endif
			}
			
			if (b2_count > 0) {
				if (++b2_phase == b2_count) {
					b2_phase = 0;
				}
			}		
			*dstPtr = (dT) val;
			srcPtr += sStep;
			dstPtr += dStep;
			
		}
	}
	
	template <class sT, class dT>	
	void gmbLine(const void *sPtr, void *dPtr, uint32 srcRow, uint32 srcCol, int32 sStep, int32 dStep, uint32 count)
	{
		sT *srcPtr = (sT *) sPtr;
		dT *dstPtr = (dT *) dPtr;
		
		int32 marginY = (int32)(fActiveArea->H()/80);
		int32 marginX = (int32)(fActiveArea->W()/120);
		int32 patchY = (srcRow - marginY)/(int32)((fActiveArea->H() -2*marginY)/4);
		if (patchY > 3) patchY = 3;
		if (patchY < 0) patchY = 0;
		int32 patchRow = srcRow - patchY*(int32)((fActiveArea->H() -2*marginY)/4) - marginY;
		
		for (uint32 j = 0; j < count; j++)
		{
			int32 val = 0;
			uint32 dstCol = srcCol + j;
			int32 patchX = (dstCol - marginX)/(int32)((fActiveArea->W() -2*marginX)/6);
			if (patchX > 5) patchX = 5;
			if (patchX < 0) patchX = 0;
			int32 patchCol = dstCol - patchX*(int32)((fActiveArea->W() -2*marginX)/6) - marginX;
			int32 patch = patchX+patchY*6;
			if ((patchRow < marginY) || (patchCol < marginX) ||
				(patchRow > ((int32)(fActiveArea->H()/4) - marginY)) || 
				(patchCol > ((int32)(fActiveArea->W()/6) - marginX))) {
				patch = 24;
			}
			uint32 case_p = (dstCol & 0x1) + ((srcRow & 0x1) << 1);
			
			val = patchRGB[kbayerColors*patch + case_p];
			if (val > intClipLevel) val = intClipLevel;
			if (rlut)
			{
				val = rlut[val];
			}
			*dstPtr = (dT) val;
			srcPtr += sStep;
			dstPtr += dStep;
			
		}
	}
	
	
};
