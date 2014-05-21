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
 * dng_devignette_negative.cpp
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
 * 12 Sept 2009 : 1.2.0.0
 * 19 Sept 2009 : 1.3.0.0
 * 9 Nov 2009 : 1.3.0.3
 * 6 October 2010 : 1.4.0.0
 * 23 January 2011 : 1.4.1.0
 *
 */

#include "dng_devignette_negative.h"
#include "dng_area_task.h"
#include "dng_linearization_info.h"
#include "dng_image.h"
#include "dng_exceptions.h"
#include "M8_devignette.h"

#define kFunctionArraySize 3

/*****************************************************************************/



class dng_devignette_plane: public dng_area_task
	{
	
	private:
	
		const dng_image & fSrcImage;
		      dng_image & fDstImage;
	      
		uint32 fPlane;
		dng_rect fActiveArea;
		      
		uint32 fSrcPixelType;
		uint16 *lut;
		int antiAliasStrength;
		M8_devignette *vFunctions;
		dng_line_processor lineProcessor;

		real32 *factorCache[kbayerColors];

	public:
	
		dng_devignette_plane (dng_host &host,
			const dng_linearization_info &info,
			const dng_image &srcImage,
			dng_image &dstImage,
			uint32 plane,
			M8_devignette *vDescriptor,
			int antiAlias,
			int chromaStrength,
			int lumaStrength,
			dng_string &cameraType,
			bool isSinglePlaneCFA);
							 
		virtual ~dng_devignette_plane ();
	
		virtual void Process (uint32 threadIndex,
							  const dng_rect &tile,
							  dng_abort_sniffer *sniffer);
								  
	};

/*****************************************************************************/
typedef	dng_lmfit_functions *dng_lmfit_functions_ptr;

dng_devignette_plane::dng_devignette_plane (dng_host &host,
											const dng_linearization_info &info,
											const dng_image &srcImage,
											dng_image &dstImage,
											uint32 plane,
											M8_devignette *vDescriptor,
											int antiAlias,
											int chromaStrength,
											int lumaStrength,
											dng_string &cameraType,
											bool isSinglePlaneCFA)

:	fSrcImage (srcImage)
,	fDstImage (dstImage)
,	fPlane (plane)
,	fActiveArea (srcImage.Bounds())
,	fSrcPixelType (srcImage.PixelType ())
,	vFunctions (vDescriptor)
,	antiAliasStrength (antiAlias)



{
	
	// Make sure the source pixel type is supported.
	
	if (fSrcPixelType != srcImage.PixelType ())
	{
		
		DNG_ASSERT (false, "Source and destination pixels must be the same");
		
		ThrowProgramError ();
		
	}
	
	if (fSrcPixelType != ttByte
//		&& fSrcPixelType != ttSByte
		&& fSrcPixelType != ttShort
//		&& fSrcPixelType != ttSShort
//		&& fSrcPixelType != ttLong
//		&& fSrcPixelType != ttSLong
//		&& fSrcPixelType != ttFloat
		)
	{
		
		DNG_ASSERT (false, "Unsupported source pixel type");
		
		ThrowProgramError ();
		
	}
	
	lineProcessor.setClipLevel((int32) info.fWhiteLevel[plane]);	
	if (info.fLinearizationTable.Get()) 
	{
		lut = info.fLinearizationTable->Buffer_uint16();
		lineProcessor.setLUT(info.fLinearizationTable->Buffer_uint16(), info.fLinearizationTable->LogicalSize() / sizeof (uint16));
		lineProcessor.buildRlut(cameraType);
	}
	else
	{
		lut = NULL;
	}
	lineProcessor.setLinearisationInfo(host, srcImage, info, plane);
	
	// Adjust maximum tile size.
	
	fMaxTileSize = dng_point (1024, 1024);
	
	// Note must call setActive area before setOffsets
	lineProcessor.setActiveArea(&fActiveArea);
	
	if (vFunctions != NULL) {
		for (uint32 octant = 0; octant < kMaxOctants; octant++) 
		{
			dng_lmfit_functions *functionSet[kFunctionArraySize];
			
			for (uint32 i = 0; i < kFunctionArraySize; i++) {
				functionSet[i] = NULL;
			}
			dng_lmfit_functions *primaryFunction = NULL;
			
			switch (vFunctions->getNumSegments()) {
				case 1:
					functionSet[0] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
					functionSet[1] = NULL;
					functionSet[2] = NULL;
					break;
				case 4:
					switch (octant) {
						case kLeftHorizonalOctant:
							functionSet[0] = NULL;
							functionSet[1] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kBottomLeftDiagonalSegment);
							break;
						case kRightHorizonalOctant:
							functionSet[0] = NULL;
							functionSet[1] = vFunctions->getFitFunctions(kTopRightDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kBottomRightDiagonalSegment);
							break;
						case kTopVerticalOctant:
							functionSet[0] = NULL;
							functionSet[1] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kTopRightDiagonalSegment);
							break;
						case kBottomVerticalOctant:
							functionSet[0] = NULL;
							functionSet[1] = vFunctions->getFitFunctions(kBottomLeftDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kBottomRightDiagonalSegment);
							break;
						case kTopLeftDiagonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						case kTopRightDiagonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kTopRightDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						case kBottomRightDiagonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kBottomRightDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						case kBottomLeftDiagonalOctant:	
							functionSet[0] = vFunctions->getFitFunctions(kBottomLeftDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						default:
							functionSet[0] = NULL;
							functionSet[1] = NULL;
							functionSet[2] = NULL;
					}
					break;
				case 8:
					switch (octant) {
						case kLeftHorizonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kLeftHorizonalSegment);
							functionSet[1] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kBottomLeftDiagonalSegment);
							break;
						case kTopLeftDiagonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						case kTopVerticalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kTopVerticalSegment);
							functionSet[1] = vFunctions->getFitFunctions(kTopLeftDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kTopRightDiagonalSegment);
							break;
						case kTopRightDiagonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kTopRightDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						case kRightHorizonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kRightHorizonalSegment);
							functionSet[1] = vFunctions->getFitFunctions(kTopRightDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kBottomRightDiagonalSegment);
							break;
						case kBottomRightDiagonalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kBottomRightDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						case kBottomVerticalOctant:
							functionSet[0] = vFunctions->getFitFunctions(kBottomVerticalSegment);
							functionSet[1] = vFunctions->getFitFunctions(kBottomLeftDiagonalSegment);
							functionSet[2] = vFunctions->getFitFunctions(kBottomRightDiagonalSegment);
							break;
						case kBottomLeftDiagonalOctant:	
							functionSet[0] = vFunctions->getFitFunctions(kBottomLeftDiagonalSegment);
							functionSet[1] = NULL;
							functionSet[2] = NULL;
							break;
						default:
							functionSet[0] = NULL;
							functionSet[1] = NULL;
							functionSet[2] = NULL;
						}
					break;
				default:
					break;
			}
			// If we are offset then the radius increases......
			int32 maxX = 0;
			int32 maxY = 0;
			for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
			{
				if (functionSet[0] != NULL) {
					maxX = Max_int32(Abs_int32(functionSet[0][case_p].getCenterX()), maxX);
					maxY = Max_int32(Abs_int32(functionSet[0][case_p].getCenterY()), maxY);
					primaryFunction = functionSet[0];
				}
				else {
					if (functionSet[1] != NULL) {
						maxX = Max_int32(Abs_int32(functionSet[1][case_p].getCenterX()), maxX);
						maxY = Max_int32(Abs_int32(functionSet[1][case_p].getCenterY()), maxY);
					}
					if (functionSet[2] != NULL) {
						maxX = Max_int32(Abs_int32(functionSet[2][case_p].getCenterX()), maxX);
						maxY = Max_int32(Abs_int32(functionSet[2][case_p].getCenterY()), maxY);
					}
					primaryFunction = functionSet[1];
				}
			}
			uint32 cacheLength = (uint32) pow (pow((real64) (fActiveArea.H()>>1) + maxY + 3, 2) + 
										   pow((real64) (fActiveArea.W()>>1) + maxX + 3 , 2), 0.5);
			real64 val = 0.0;
			int refSelect = 0;
			
			switch (primaryFunction[0].getCorrectionType()) {
				case normalCorrection:
					for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
					{
						if (primaryFunction[case_p].evalSelected(cacheLength - 100) > val)
						{
							val = primaryFunction[case_p].evalSelected(cacheLength - 100);
							refSelect = case_p;
						}
					}
					break;
				case differentialCorrection:
					refSelect = referenceBayerColor;
				default:
					break;
			}


			for (uint32 case_p = 0; case_p < kbayerColors; case_p++) {
				for (uint32 i = 0; i < kFunctionArraySize; i++) {
					if (functionSet[i] != NULL) {
						functionSet[i][case_p].setTweakOn(true);
						functionSet[i][case_p].setRefFunction(&(functionSet[i][refSelect]));
						functionSet[i][case_p].setChromaTweak(((real64) chromaStrength)/100.0);
						functionSet[i][case_p].setLuminanceTweak(((real64) lumaStrength)/100.0);
					}
				}
				// Note must call setActive area before setOffsets
				lineProcessor.setOffsets(case_p, primaryFunction[case_p].getCenterX(), primaryFunction[case_p].getCenterY());
			}
			lineProcessor.buildFactorCache(octant, isSinglePlaneCFA, plane, functionSet[0], functionSet[1], functionSet[2], cacheLength);
		}

	}
		
	fMaxThreads = 16;
	
	lineProcessor.setAntiAlias(antiAlias);
	
}

/*****************************************************************************/

dng_devignette_plane::~dng_devignette_plane ()
	{
	}
							 
						 
/*****************************************************************************/

void dng_devignette_plane::Process (uint32 /* threadIndex */,
							  	   const dng_rect &srcTile,
							  	   dng_abort_sniffer * /* sniffer */)
	{

	// Process tile.
	
	dng_rect dstTile = srcTile - fActiveArea.TL ();

	dng_const_tile_buffer srcBuffer (fSrcImage, srcTile);
	dng_dirty_tile_buffer dstBuffer (fDstImage, dstTile);
	
	int32 sStep = srcBuffer.fColStep;
	int32 dStep = dstBuffer.fColStep;
	uint32 count = srcTile.W ();
	
//	uint32 srcCol = srcTile.l;

	uint32 rows = srcTile.H ();

	for (uint32 row = 0; row < rows; row++)
		{

		uint32 srcRow = srcTile.t + row;
	
	
		const void *sPtr = srcBuffer.ConstPixel (srcTile.t + row,
											     srcTile.l,
											     fPlane);
	
		void *dPtr = dstBuffer.DirtyPixel (dstTile.t + row,
										   dstTile.l,
										   fPlane);

		switch (fSrcPixelType)
		{
		case ttByte:
			lineProcessor.processLine<uint8, uint8>(sPtr, dPtr, srcRow, dstTile.l, sStep, dStep, count, fSrcImage.Bounds().t, fSrcImage.Bounds().l);
			break;
		case ttShort:
			lineProcessor.processLine<uint16, uint16>(sPtr, dPtr, srcRow, dstTile.l, sStep, dStep, count, fSrcImage.Bounds().t, fSrcImage.Bounds().l);
			break;
		}

	}
						
}
	

dng_devignette_negative::dng_devignette_negative(void)
{
	chromaStrength = 100;
	lumaStrength = 100;
}
dng_devignette_negative::~dng_devignette_negative(void)
{


}


void dng_devignette_negative::SetChromaStrength(int strength)
{
	chromaStrength = strength;
}

void dng_devignette_negative::SetLumaStrength(int strength)
{
	lumaStrength = strength;
}



void dng_devignette_negative::doDeVignette (dng_host &host,
										const dng_linearization_info &info,
										const dng_image &srcImage,
										dng_image &dstImage,
										M8_devignette *vDescriptor,
										int antiAliasStrength,
										dng_string &cameraType)
	{
	
	// Process each image plane separately.
	
	for (uint32 plane = 0; plane < srcImage.Planes (); plane++)
		{
		
		// Build linearization table for this plane.
		
		dng_devignette_plane processor (host,
									   info,
									   srcImage,
									   dstImage,							   
									   plane, 
									   vDescriptor,
									   antiAliasStrength,
									   chromaStrength,
									   lumaStrength,
									   cameraType,
										srcImage.Planes() == 1 );
									   
		// Process plane.
		
		host.PerformAreaTask (processor, info.fActiveArea);
						
		}
	
}


