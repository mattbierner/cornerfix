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
 * dng_macbeth_info.cpp
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
 *
 */

#include "dng_line_processor.h"
#include "dng_host.h"
#include "dng_area_task.h"
#include "dng_image.h"
#include "dng_exceptions.h"
#include "dng_info.h"
#include "dng_lmfit_functions.h"
#include "dng_macbeth_info.h"



class dng_Macbeth_plane: public dng_area_task
	{
	
	private:
	
		const dng_image & fSrcImage;
		      dng_image & fDstImage;
		      
		uint32 fPlane;
		dng_rect fActiveArea;
		      
		uint32 fSrcPixelType;
		uint32 fDstPixelType;
		uint16 *lut;


		dng_line_processor lineProcessor;


	public:
	
		dng_Macbeth_plane (dng_host &host,
							 dng_linearization_info &info,
							 const dng_image &srcImage,
							 dng_image &dstImage,
							 uint32 plane,
							 uint16 *rgbLookup,
							 dng_string &cameraType);
							 
		virtual ~dng_Macbeth_plane ();
	
		virtual void Process (uint32 threadIndex,
							  const dng_rect &tile,
							  dng_abort_sniffer *sniffer);
		void setAntiAlias(int antiAlias);
								  
	};

/*****************************************************************************/

dng_Macbeth_plane::dng_Macbeth_plane (dng_host &host,
										  dng_linearization_info &info,
										  const dng_image &srcImage,
										  dng_image &dstImage,
										  uint32 plane,
										  uint16 *rgbLookup,
										  dng_string &cameraType)
							 
	:	fSrcImage (srcImage)
	,	fDstImage (dstImage)
	,	fPlane (plane)
	,	fActiveArea (srcImage.Bounds())
	,	fSrcPixelType (srcImage.PixelType ())
	,	fDstPixelType (dstImage.PixelType ())

	{

	// Make sure the source pixel type is supported.
	
	if (fSrcPixelType != ttByte  &&
		fSrcPixelType != ttShort)
		{
		
		DNG_ASSERT (false, "Unsupported source pixel type");
		
		ThrowProgramError ();
		
		}
		
	if (fSrcPixelType != ttByte  &&
		fSrcPixelType != ttShort)
		{
		
		DNG_ASSERT (false, "Unsupported destination pixel type");
		
		ThrowProgramError ();
		
		}

	if (info.fLinearizationTable.Get()) {
		lut = info.fLinearizationTable->Buffer_uint16();
		lineProcessor.setLUT(info.fLinearizationTable->Buffer_uint16(), info.fLinearizationTable->LogicalSize() / sizeof (uint16));	
		lineProcessor.setClipLevel((int32) info.fWhiteLevel[plane]);
		lineProcessor.buildRlut(cameraType);
	}
	else {
		lut = NULL;
	}

	lineProcessor.setPatchRGB(rgbLookup);

	lineProcessor.setActiveArea(&fActiveArea);
		
	fMaxThreads = 16;
	
	// Adjust maximum tile size.
		
	fMaxTileSize = dng_point (1024, 1024);
		
	}
							 
/*****************************************************************************/

dng_Macbeth_plane::~dng_Macbeth_plane ()
	{
	
	}
							 
						 
/*****************************************************************************/



void dng_Macbeth_plane::Process (uint32 /* threadIndex */,
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
	
	uint32 dstCol = dstTile.l;

	uint32 rows = srcTile.H ();


	for (uint32 row = 0; row < rows; row++)
		{

		uint32 dstRow = dstTile.t + row;
	
		const void *sPtr = srcBuffer.ConstPixel (srcTile.t + row,
											     srcTile.l,
											     fPlane);
	
		void *dPtr = dstBuffer.DirtyPixel (dstRow,
										   dstCol,
										   fPlane);

		switch (fDstPixelType) {
			case ttByte:

				switch (fSrcPixelType)
				{
				case ttByte:
					lineProcessor.gmbLine<uint8,uint8>(sPtr, dPtr, dstRow, dstTile.l, sStep, dStep, count);
					break;
				case ttShort:
				case ttSShort:
					lineProcessor.gmbLine<uint16,uint8>(sPtr, dPtr, dstRow, dstTile.l, sStep, dStep, count);
					break;
				}
				break;

			case ttShort:
			case ttSShort:

				switch (fSrcPixelType)
				{
				case ttByte:
					lineProcessor.gmbLine<uint8,uint16>(sPtr, dPtr, dstRow, dstTile.l, sStep, dStep, count);
					break;
				case ttShort:
					lineProcessor.gmbLine<uint16,uint16>(sPtr, dPtr, dstRow, dstTile.l, sStep, dStep, count);
					break;
				}
				break;

		}

		}		
	}
	


dng_macbeth_info::dng_macbeth_info(void)
{

}

dng_macbeth_info::~dng_macbeth_info(void)
{
}



void dng_macbeth_info::initFromLinearizationInfo(dng_linearization_info *info, dng_host &host, GretagMacBeth *gretag)
{

		fActiveArea = info->fActiveArea;

		/// Number of rectangles in fMaskedArea
		// We don't care

		fMaskedAreaCount = 0;

		for (int i = 0; i < NUMGretagPatches+1; i++) {
			for (int j = 0; j < kMaxSamplesPerPixel; j++) {
				int color;
				switch (j) {
					 // M8 and Canon
					case 0: color = 0;
						break;
					case 1: color = 1;
						break;
					case 2: color = 1;
						break;
					case 3: color = 2;
						break;

					/*
					// Nikon D80. etc
					case 0: color = 1;
						break;
					case 1: color = 2;
						break;
					case 2: color = 0;
						break;
					case 3: color = 1;
						break;
					 */

					default: color = 0;
				}
				// make the heroic assumption we only have one plane.....
				uint32 col = j & 0x1;
				uint32 row = (j & 0x2) >> 1;
				real64 val = gretag->getpatchRGB(i)[color] * 
							  (info->fWhiteLevel[0] - info->fBlackLevel[row][col][0]) 
							  + info->fBlackLevel[row][col][0];
				// Clip to whitelevel
				patchLookup[i][j]= (uint16) (val > info->fWhiteLevel[0] ? info->fWhiteLevel[0] : val);
			}
		}			
	
		/// A lookup table that maps stored values into linear values.
		/// This tag is typically used to increase compression ratios by storing the raw data in a non-linear, more
		/// visually uniform space with fewer total encoding levels.
		/// If SamplesPerPixel is not equal to one, e.g. Fuji S3 type sensor, this single table applies to all the samples for each
		/// pixel.
		if (info->fLinearizationTable.Get()) {
			//Logical size gives us bytes
			uint32 size = info->fLinearizationTable.Get()->LogicalSize();
		
			fLinearizationTable.Reset (host.Allocate (size));
														  
			uint16 *table = fLinearizationTable->Buffer_uint16 ();
			uint16 *source = info->fLinearizationTable.Get()->Buffer_uint16 ();
				
			for (uint32 j = 0; j < size/(sizeof(uint16)); j++)
				{
				table [j] = source[j];
				}
		}


		/// Actual number of rows in fBlackLevel pattern

		fBlackLevelRepeatRows = 0;

		/// Actual number of columns in fBlackLevel pattern

		fBlackLevelRepeatCols = 0;
}

void dng_macbeth_info::doMacbeth (dng_host &host,
										const dng_image &srcImage,
										dng_image &dstImage, 
										dng_string &cameraType)
	{
	
	// Process each image plane separately.
	
	for (uint32 plane = 0; plane < srcImage.Planes (); plane++)
		{
		
		// Build linearization table for this plane.
		
		dng_Macbeth_plane processor (host,
									   *this,
									   srcImage,
									   dstImage,
									   plane,
									   &patchLookup[0][0],
									   cameraType);
									   
		// Process plane.
		
		host.PerformAreaTask (processor, fActiveArea);
						
		}

}



