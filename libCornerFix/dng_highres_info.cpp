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
 * dng_hightres_info.cpp
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
 * 12 Sept 2009 : 1.2.0.0
 * 19 Sept 2009 : 1.3.0.0
 * 23 January 2011 : 1.4.1.0
 *
 */
#include "dng_highres_info.h"
#include "dng_host.h"
#include "dng_area_task.h"
#include "dng_image.h"
#include "dng_exceptions.h"
#include "dng_info.h"
#include "dng_lmfit_functions.h"
#include "dng_line_processor.h"



class dng_hires_plane: public dng_area_task
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
	
		dng_hires_plane (dng_host &host,
							 dng_linearization_info &info,
							 const dng_image &srcImage,
							 dng_image &dstImage,
							 uint32 plane);
							 
		virtual ~dng_hires_plane ();
	
		virtual void Process (uint32 threadIndex,
							  const dng_rect &tile,
							  dng_abort_sniffer *sniffer);
		void setAntiAlias(int antiAlias);
								  
	};

/*****************************************************************************/

dng_hires_plane::dng_hires_plane (dng_host &host,
										  dng_linearization_info &info,
										  const dng_image &srcImage,
										  dng_image &dstImage,
										  uint32 plane)
							 
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
		
	if (fDstPixelType != ttShort)
		{
		
		DNG_ASSERT (false, "Unsupported destination pixel type");
		
		ThrowProgramError ();
		
		}

	if (info.fLinearizationTable.Get()) {
		lut = info.fLinearizationTable->Buffer_uint16();
	}
	else {
		lut = NULL;
	}
	lineProcessor.setClipLevel((int32) info.fWhiteLevel[plane]);
	lineProcessor.setLinearisationInfo(host, srcImage, info, plane);
		
	lineProcessor.setActiveArea(&fActiveArea);

	if (lut)
	{
		lineProcessor.setLUT(info.fLinearizationTable->Buffer_uint16(), info.fLinearizationTable->LogicalSize() / sizeof (uint16));
	}

	
	// Adjust maximum tile size.
		
	fMaxTileSize = dng_point (1024, 1024);
	fMaxThreads = 16;
		
	}
							 
/*****************************************************************************/

dng_hires_plane::~dng_hires_plane ()
	{
	
	}
							 
						 
/*****************************************************************************/

void dng_hires_plane::setAntiAlias(int antiAlias)
{
	lineProcessor.setAntiAlias(antiAlias);

}



void dng_hires_plane::Process (uint32 /* threadIndex */,
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

		switch (fSrcPixelType)
		{
		case ttByte:
			lineProcessor.processLine<uint8,uint16>(sPtr, dPtr, 0, 0, sStep, dStep, count, fSrcImage.Bounds().t, fSrcImage.Bounds().l);
			break;
		case ttShort:
			lineProcessor.processLine<uint16,uint16>(sPtr, dPtr, 0, 0, sStep, dStep, count, fSrcImage.Bounds().t, fSrcImage.Bounds().l);
			break;
		}

		}		
	}
	

dng_highres_info::dng_highres_info(void)
{
		antiAliasStrength = 0;
}

dng_highres_info::dng_highres_info(dng_linearization_info *info, dng_host &host, bool maximizeResolution)
{
	
	fActiveArea = info->fActiveArea;
	fMaskedAreaCount = info->fMaskedAreaCount;
	for (int i = 0; i < kMaxMaskedAreas; i++) {
		fMaskedArea[i] = info->fMaskedArea[i];
	}
	
	fBlackLevelRepeatRows = info->fBlackLevelRepeatRows;
	
	fBlackLevelRepeatCols = info->fBlackLevelRepeatCols;

	for (uint32 n = 0; n < kMaxSamplesPerPixel; n++) {
		fWhiteLevel[n] = info->fWhiteLevel[n];
		for (uint32  j = 0; j < fBlackLevelRepeatRows; j++)
		{
			for (uint32  k = 0; k < fBlackLevelRepeatCols; k++)
			{
				fBlackLevel[j][k][n] = info->fBlackLevel[j][k][n];
			}
		}
	}
	
	// Now we update the previous table
	
	int shiftVal = 0;
	if (maximizeResolution) {
		real64 maxWhite = 255;
		for (uint32  i = 0; i < kMaxSamplesPerPixel; i++) {
			if (maxWhite < info->fWhiteLevel[i]) maxWhite = info->fWhiteLevel[i];
		}
		while (maxWhite < 0x8000) {
			maxWhite *= 2;
			shiftVal++;
		}
		//			dng_local_printf::printf("shiftVal: %i\n", shiftVal);
		for (uint32 n = 0; n < kMaxSamplesPerPixel; n++) {
			info->fWhiteLevel[n] = info->fWhiteLevel[n] * pow(2.0,shiftVal);
			fWhiteLevel[n] = info->fWhiteLevel[n];
			for (uint32  j = 0; j < fBlackLevelRepeatRows; j++)
			{
				for (uint32  k = 0; k < fBlackLevelRepeatCols; k++)
				{
					info->fBlackLevel[j][k][n] = info->fBlackLevel[j][k][n] * pow(2.0,shiftVal);
					fBlackLevel[j][k][n] = info->fBlackLevel[j][k][n];
				}
			}
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
			table [j] = source[j] << shiftVal;
			}
	}
}

dng_highres_info::~dng_highres_info(void)
{
}

void dng_highres_info::setAntiAlias(int antiAlias)
{
	antiAliasStrength = antiAlias;
}

void dng_highres_info::setCameraType(dng_string &type)
{

}

void dng_highres_info::doHires (dng_host &host,
										const dng_image &srcImage,
										dng_image &dstImage)
	{
	
	// Process each image plane separately.
	
	for (uint32 plane = 0; plane < srcImage.Planes (); plane++)
		{
		
		// Build linearization table for this plane.
		
		dng_hires_plane processor (host,
									   *this,
									   srcImage,
									   dstImage,
									   plane);
									   
		// Process plane.

		processor.setAntiAlias(antiAliasStrength);
		
		host.PerformAreaTask (processor, fActiveArea);
						
		}

}









