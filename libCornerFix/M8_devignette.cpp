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
 * M8_devignette.cpp
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 01 Jun 2007 : Original version;
 * 05 Jun 2007 : Fix bug when multiple profiles generated
 * 18 Jun 2007 : 0.9.0.5
 * 22 Jun 2007 : 0.9.0.6
 * 1 Jul 2007 : 0.9.1.0
 * 1 Sept 2007 : 0.9.1.0 Mac version
 * 12 Sept 2009 : 1.2.0.0
 * 14 Oct 2009 : 1.3.0.0
 * 21 Oct 2009 : 1.3.0.1
 * 9 Nov 2009 : 1.3.0.3
 * 28 Nov 2009 : 1.3.0.4
 * 6 October 2010 : 1.4.0.0
 * 22 October 2010 : 1.4.0.2
 * 23 January 2011 : 1.4.1.0
 * 4 July 2011 : 1.4.2.0
 * 4 December 2013 : 1.4.2.6
 *
 */

/*
 * Handles reference image analysis, and 
 * devignetting of images
 */

#include "dng_auto_ptr.h"
#include "dng_classes.h"
#include "dng_memory.h"
#include "dng_rational.h"
#include "dng_rect.h"
#include "dng_sdk_limits.h"
#include "dng_mutex.h"
#include "dng_globals.h"
#include "M8_devignette.h"
#include "dng_lmfit.h"
#include "dng_file_string.h"
#include "dng_area_task.h"
#include "dng_linearization_info.h"
#include "dng_image.h"
#include "dng_exceptions.h"
#include "math.h"
#include "dng_host.h"


//If this defined, a synthetic image is generated
//#define gen_synthetic

// The type of equation used to generate the lenses center
#define kLensCenterEquationType 11

#define cacheMode

#ifdef arrayCheck
#pragma message("WARNING - Compiling arrayCheck Options in")
#endif

//////////////////////////////////////////////////////////////
//
// IllumComp class
//
//////////////////////////////////////////////////////////////

IllumComp::IllumComp() {

	clearComp();
	
}

IllumComp::~IllumComp() {


}

void IllumComp::clearComp() 
{
	for (uint32 i = 0; i < kbayerColors; i++) {

		mRow[i] = 0.0;
		mCol[i] = 0.0;
		bRow[i] = 1.0;
		bCol[i] = 1.0;
		bBoth[i] = 1.0;
		maxPixelval[i] = 0.0;
		kmax[i] = 1.0;
	}
}

real64 IllumComp::getmRow(uint32 color) 
{
	return mRow[color];
}
real64 IllumComp::getmCol(uint32 color) 
{
	return mCol[color];
}
real64 IllumComp::getbRow(uint32 color) 
{
	return bRow[color];
}
real64 IllumComp::getbCol(uint32 color) 
{
	return bCol[color];
}
real64 IllumComp::getbBoth(uint32 color) 
{
	return bBoth[color];
}
void IllumComp::setmRow(uint32 color, real64 val) 
{
	mRow[color] = val;
}
void IllumComp::setmCol(uint32 color, real64 val) 
{
	mCol[color] = val;
}
void IllumComp::setbRow(uint32 color, real64 val) 
{
	bRow[color] = val;
}
void IllumComp::setbCol(uint32 color, real64 val) 
{
	bCol[color] = val;
}
void IllumComp::setbBoth(uint32 color, real64 val) 
{
	bBoth[color] = val;
}
real64 IllumComp::getmaxPixelval(uint32 color)
{
	return maxPixelval[color];
}
void IllumComp::setmaxPixelval(uint32 color, real64 val) 
{
	maxPixelval[color] = val;
}
real64 IllumComp::getK(uint32 row, uint32 col, uint32 color)
{
	
	real64 r;
	r = (mRow[color]*((real64) row) + mCol[color]*((real64) col)) / bBoth[color] + 1.00;
	return r;
}

//////////////////////////////////////////////////////////////
//
// DataStore class
//
//////////////////////////////////////////////////////////////


DataStore::DataStore(dng_pixel_buffer &pixBuffer) {

	clearBuffer(pixBuffer);
	
}

DataStore::~DataStore() {


}

void DataStore::clearBuffer(dng_pixel_buffer &pixBuffer)
{
	int dataSize = 0;
	// Initialise the databuffer for a normal array of analysis data
	if (pixBuffer.fData != NULL) 
	{
		// Space for two diagonals
		real64 hQuads = (real64) ((pixBuffer.fArea.H()>>1) + 1);
		real64 wQuads = (real64) ((pixBuffer.fArea.W()>>1) + 1);
		dataSize = ((int) sqrt(pow(hQuads,2)+ pow(wQuads, 2)))*2 + 1;
		// and then horizontal and vertical
		dataSize += ((int) hQuads) + ((int) wQuads) + 1;
	}
	
	clearBuffer(pixBuffer, dataSize);
}

void DataStore::clearBuffer(dng_pixel_buffer &pixBuffer, int dataSize)
{
	dataArray.setSize(kbayerColors, kdimensions, dataSize);
	for (uint32 i = 0; i < kbayerColors; i++) {
		dataLength[i] = 0;
		bufferMax[i] = 0;
		for (uint32 j = 0; j < kdimensions; j++)
			for (uint32 k = 0; k < (uint32) dataArray.getZSize(); k++)
				dataArray.set(i, j, k, 0);
	}
	for (uint32 l = 0; l < kMaxSegments; l++) {
		segmentLength[l] = 0;
	}
}

int DataStore::elementsIn(void) 
{
	return dataArray.elementsIn();
}

void DataStore::addData(uint32 color, uint32 row, uint32 col, uint32 p_data) {

	dataArray.set(color, 0, dataLength[color], row);
	dataArray.set(color, 1, dataLength[color], col);
	dataArray.set(color, 2, dataLength[color], p_data);
	if (p_data > bufferMax[color]){
		bufferMax[color] = p_data;
	}
	dataLength[color]++;

}

void DataStore::addQuad(dng_pixel_buffer &pixBuffer, uint32 row, uint32 col, int colorToPlane[]) {


	if ((row < pixBuffer.fArea.H()-1) && (col < pixBuffer.fArea.W()-1))
	{
        if (cropHeighti > 0 && cropWidthi > 0) {
            col = Max_uint32(col, cropOrigHi);
            col = Min_uint32(col, cropOrigHi+cropWidthi);
            row = Max_uint32(row, cropOrigVi);
            row = Min_uint32(row, cropOrigVi+cropHeighti);
        }
        
		//Ensure that we're on the border of a quad......
		row &= 0xfffe;
		col &= 0xfffe;

		switch (pixBuffer.fPixelType) {

			case ttByte:
			case ttSByte:

				const uint8 * pixel_ptr8;
				pixel_ptr8 = pixBuffer.ConstPixel_uint8(row, col, colorToPlane[color_red]);
				addData(color_red, row, col, (uint32) *pixel_ptr8);
				pixel_ptr8 = pixBuffer.ConstPixel_uint8(row, col+1, colorToPlane[color_green1]);
				addData(color_green1, row, col+1, (uint32) *pixel_ptr8);
				pixel_ptr8 = pixBuffer.ConstPixel_uint8(row+1, col, colorToPlane[color_green2]);
				addData(color_green2, row+1, col, (uint32) *pixel_ptr8);
				pixel_ptr8 = pixBuffer.ConstPixel_uint8(row+1, col+1, colorToPlane[color_blue]);
				addData(color_blue, row+1, col+1, (uint32) *pixel_ptr8);

				break;

			case ttShort:
			case ttSShort:

				const uint16 * pixel_ptr16;
				pixel_ptr16 = pixBuffer.ConstPixel_uint16(row, col, colorToPlane[color_red]);
				addData(color_red, row, col, (uint32) *pixel_ptr16);
				pixel_ptr16 = pixBuffer.ConstPixel_uint16(row, col+1, colorToPlane[color_green1]);
				addData(color_green1, row, col+1, (uint32) *pixel_ptr16);
				pixel_ptr16 = pixBuffer.ConstPixel_uint16(row+1, col, colorToPlane[color_green2]);
				addData(color_green2, row+1, col, (uint32) *pixel_ptr16);
				pixel_ptr16 = pixBuffer.ConstPixel_uint16(row+1, col+1, colorToPlane[color_blue]);
				addData(color_blue, row+1, col+1, (uint32) *pixel_ptr16);

				break;


		}
	}


}

uint32 DataStore::getDataLength(uint32 color)
{
	return dataLength[color];

}


uint32 DataStore::getRow(uint32 color, uint32 index)
{

	return dataArray.get(color, 0, index);

}

uint32 DataStore::getCol(uint32 color, uint32 index)
{

	return dataArray.get(color, 1, index);

}

uint32 DataStore::getData(uint32 color, uint32 index)
{

	return dataArray.get(color, 2, index);

}

real64 DataStore::getCompData(uint32 color, uint32 index)
{

	return ((real64) getData(color, index));

}

int32 DataStore::getBufferMax(uint32 color)
{

	return bufferMax[color];

}

void DataStore::markSegmentStart(uint32 segmentSelect)
{
	segmentLength[segmentSelect] = dataLength[0];
}

uint32 DataStore::getSegmentStart(uint32 segmentSelect)
{
	return segmentSelect < kMaxSegments ? segmentLength[segmentSelect] : 0;
	
}

uint32 DataStore::getSegmentLength(uint32 segmentSelect)
{
	uint32 retVal = 0;
	switch (segmentSelect) {
		case kAllSegments:
			retVal = getDataLength(0);
			break;
		default:
			retVal =  (((segmentSelect == (kMaxSegments - 1)) || (segmentLength[segmentSelect+1] == 0)) ? dataLength[0] : segmentLength[segmentSelect+1]) - segmentLength[segmentSelect];
			break;
	}
	return retVal;
}

void DataStore::setCropArea(real64 originH, real64 originV, real64 width, real64 height)
{
    cropOrigHi = (uint32) originH;
    cropOrigVi = (uint32) originV;
    cropWidthi = (uint32) width;
    cropHeighti = (uint32) height;
}


//////////////////////////////////////////////////////////////
//
// M8_devignette class
//
//////////////////////////////////////////////////////////////


M8_devignette::M8_devignette(void)
{
	for (int i = 0; i < kbayerColors; i++) 
	{
		factorBuffer[i] = NULL;
		radius[i]  = NULL;
	}
	dataBuffer = new DataStore(pixBuffer);
	illumBuffer = new IllumComp();
	antiAliasStrength = 0;
	doOffsets = true;
	numSegments = 8;
	BayerGreenSplit = kMaxGreenSplit;

}

M8_devignette::~M8_devignette(void)
{
	if (dataBuffer) delete dataBuffer;
	if (illumBuffer) delete illumBuffer;
	for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
	{
		if (factorBuffer[case_p]) delete [](factorBuffer[case_p]);
		if (radius[case_p]) delete [](radius[case_p]);
	}
}

void M8_devignette::SetAntiAlias(int strength)
{
	antiAliasStrength = strength;
}

int M8_devignette::GetAntiAlias(void)
{
	return antiAliasStrength;
}


void M8_devignette::SetAsymetry(real64 strength)
{
	asymetry = strength;
}

real64 M8_devignette::GetAsymetry(void)
{
	return asymetry;
}

dng_lmfit_functions *M8_devignette::getFitFunctions(int segment)
{
#ifdef _DEBUG
	if (function[segment] == NULL) {
		DNG_ASSERT (false, "Function is NULL");
	}
#endif
	return function[segment];

}

void M8_devignette::setNumSegments(uint32 val)
{
	numSegments = val;
}

uint32 M8_devignette::getNumSegments(void)
{
	return numSegments;
}


void M8_devignette::SetStage2Buffer (const dng_pixel_buffer * buffer)

{
	dataBuffer->clearBuffer(pixBuffer);
}

void M8_devignette::setCropArea(real64 originH, real64 originV, real64 width, real64 height)
{
    cropOrigH = originH;
    cropOrigV = originV;
    cropWidth = width;
    cropHeight = height;
    if (dataBuffer) {
        dataBuffer->setCropArea(cropOrigH, cropOrigV, cropWidth, cropHeight);
    }
}

void M8_devignette::SetStage2Buffer (dng_simple_image * image)
{
	image->GetPixelBuffer (pixBuffer);
	dataBuffer->clearBuffer(pixBuffer);
}

real64 M8_devignette::getVignetteFactor(uint32 index)
{
	uint32 row = dataBuffer->getRow(0, index);
	uint32 col = dataBuffer->getCol(0, index);
	uint32 case_p = (col & 0x1) + ((row & 0x1) << 1);

	real64 r = dataBuffer->getData(case_p, index)/illumBuffer->getmaxPixelval(case_p)/illumBuffer->getK(row, col, case_p);

	return r;
}

void M8_devignette::correctIluminant()
{
	real64 ave_row[kbayerColors];
	real64 ave_col[kbayerColors];
	real64 ave_val[kbayerColors];
	real64 topSigmaRow[kbayerColors];
	real64 topSigmaCol[kbayerColors];
	real64 bottomSigmaRow[kbayerColors];
	real64 bottomSigmaCol[kbayerColors];
	int32 case_p = 0;
	uint32 col;
	uint32 row;
	real64 N = 0.0;
	for (int i = 0; i < kbayerColors; i++) 
	{
		ave_row[i] = 0.0;
		ave_col[i] = 0.0;
		ave_val[i] = 0.0;
		topSigmaRow[i] = 0.0;
		topSigmaCol[i] = 0.0;
		bottomSigmaRow[i] = 0.0;
		bottomSigmaCol[i] = 0.0;
	}

	if (pixBuffer.fData != NULL) {

		BuildDataBuffer();

		// So here we have the data in an array. First sort out the illuminant:
		for (uint32 color = 0; color < kbayerColors; color++) {
			for (uint32 i = 0; i < dataBuffer->getDataLength(0); i++) {
				row = dataBuffer->getRow(color, i);
				col = dataBuffer->getCol(color, i);

				case_p = (col & 0x1) + ((row & 0x1) << 1);

				switch (pixBuffer.fPixelType) {

					case ttByte:
					case ttSByte:
						const uint8 * pixel_ptr8;
						pixel_ptr8 = pixBuffer.ConstPixel_uint8(row, col);
						ave_row[case_p] = ave_row[case_p] + row;
						ave_col[case_p] = ave_col[case_p] + col;
						ave_val[case_p] = ave_val[case_p] + (real64) (*pixel_ptr8);
						if (illumBuffer->getmaxPixelval(case_p) < (real64) (*pixel_ptr8)) illumBuffer->setmaxPixelval(case_p, (real64) (*pixel_ptr8));
						N += 1.0;


						break;

					case ttShort:
					case ttSShort:

						const uint16 * pixel_ptr16;
						pixel_ptr16 = pixBuffer.ConstPixel_uint16(row, col);
						ave_row[case_p] = ave_row[case_p] + row;
						ave_col[case_p] = ave_col[case_p] + col;
						ave_val[case_p] = ave_val[case_p] + (real64) (*pixel_ptr16);
						if (illumBuffer->getmaxPixelval(case_p) < (real64) (*pixel_ptr16)) illumBuffer->setmaxPixelval(case_p, (real64) (*pixel_ptr16));
						N += 1.0;

						break;

				}
			}

		}

		N= N/kbayerColors;

		for (case_p = 0; case_p < kbayerColors; case_p++) {
			ave_row[case_p] = ave_row[case_p]/N;
			ave_col[case_p] = ave_col[case_p]/N;
			ave_val[case_p] = ave_val[case_p]/N;
		}

		for (uint32 color = 0; color < kbayerColors; color++) {
			for (uint32 i = 0; i < dataBuffer->getDataLength(0); i++) {
				row = dataBuffer->getRow(color, i);
				col = dataBuffer->getCol(color, i);

				case_p = (col & 0x1) + ((row & 0x1) << 1);

				switch (pixBuffer.fPixelType) {

					case ttByte:
					case ttSByte:

						const uint8 * pixel_ptr8;
						pixel_ptr8 = pixBuffer.ConstPixel_uint8(row, col);
						topSigmaRow[case_p] += (((real64) row) - ave_row[case_p])*(((real64) *pixel_ptr8) - ave_val[case_p])/illumBuffer->getmaxPixelval(case_p);
						topSigmaCol[case_p] += (((real64) col) - ave_col[case_p])*(((real64) *pixel_ptr8) - ave_val[case_p])/illumBuffer->getmaxPixelval(case_p);
						bottomSigmaRow[case_p] += (((real64) row) - ave_row[case_p])*(((real64) row) - ave_row[case_p]);
						bottomSigmaCol[case_p] += (((real64) col) - ave_col[case_p])*(((real64) col) - ave_col[case_p]);

						break;

					case ttShort:
					case ttSShort:

						const uint16 * pixel_ptr16;
						pixel_ptr16 = pixBuffer.ConstPixel_uint16(row, col);
						topSigmaRow[case_p] += (((real64) row) - ave_row[case_p])*(((real64) *pixel_ptr16) - ave_val[case_p])/illumBuffer->getmaxPixelval(case_p);
						topSigmaCol[case_p] += (((real64) col) - ave_col[case_p])*(((real64) *pixel_ptr16) - ave_val[case_p])/illumBuffer->getmaxPixelval(case_p);
						bottomSigmaRow[case_p] += (((real64) row) - ave_row[case_p])*(((real64) row) - ave_row[case_p]);
						bottomSigmaCol[case_p] += (((real64) col) - ave_col[case_p])*(((real64) col) - ave_col[case_p]);

						break;

				}
			}

		}

		for (case_p = 0; case_p < kbayerColors; case_p++) {
			illumBuffer->setmRow(case_p, topSigmaRow[case_p]/bottomSigmaRow[case_p]);
			illumBuffer->setmCol(case_p, topSigmaCol[case_p]/bottomSigmaCol[case_p]);
			illumBuffer->setbRow(case_p, ave_val[case_p]/illumBuffer->getmaxPixelval(case_p) - illumBuffer->getmRow(case_p)*ave_row[case_p]);
			illumBuffer->setbCol(case_p, ave_val[case_p]/illumBuffer->getmaxPixelval(case_p) - illumBuffer->getmCol(case_p)*ave_col[case_p]);
			illumBuffer->setbBoth(case_p, ave_val[case_p]/illumBuffer->getmaxPixelval(case_p) - illumBuffer->getmCol(case_p)*ave_col[case_p] - illumBuffer->getmRow(case_p)*ave_row[case_p]);
		}

	}

#if qDNGValidate
	if (gVerbose || true)
	{
		dng_local_printf::printf("\n Illuminant Factors mRow: %9.3le, %9.3le, %9.3le, %9.3le", illumBuffer->getmRow(0), illumBuffer->getmRow(1), illumBuffer->getmRow(2), illumBuffer->getmRow(3));
		dng_local_printf::printf("\n                    mCol: %9.3le, %9.3le, %9.3le, %9.3le", illumBuffer->getmCol(0), illumBuffer->getmCol(1), illumBuffer->getmCol(2), illumBuffer->getmCol(3));
		dng_local_printf::printf("\n                    bRow: %9.3le, %9.3le, %9.3le, %9.3le", illumBuffer->getbRow(0), illumBuffer->getbRow(1), illumBuffer->getbRow(2), illumBuffer->getbRow(3));
		dng_local_printf::printf("\n                    bCol: %9.3le, %9.3le, %9.3le, %9.3le", illumBuffer->getbCol(0), illumBuffer->getbCol(1), illumBuffer->getbCol(2), illumBuffer->getbCol(3));
		dng_local_printf::printf("\n                    bBoth: %9.3le, %9.3le, %9.3le, %9.3le", illumBuffer->getbBoth(0), illumBuffer->getbBoth(1), illumBuffer->getbBoth(2), illumBuffer->getbBoth(3));
		dng_local_printf::printf("\n");
	}
#endif

}

void M8_devignette::BuildDataBuffer(void)
{
	//Only build if it don't exist..........
	int colorToPlane[kbayerColors];
	
	if (pixBuffer.Planes() > 1) {
		// This has to be an 3 plane RGB layout
		colorToPlane[color_red] = 0;
		colorToPlane[color_green1] = 1;
		colorToPlane[color_green2] = 1;
		colorToPlane[color_blue] = 2;
	}
	else {
		// Must be a single plane CFA layout
		colorToPlane[color_red] = 0;
		colorToPlane[color_green1] = 0;
		colorToPlane[color_green2] = 0;
		colorToPlane[color_blue] = 0;		
	}
	if ((pixBuffer.fData != NULL) && (dataBuffer->getDataLength(0) < 1)) {
		uint32 row, col;
		dataBuffer->clearBuffer(pixBuffer);
		// increment by two to move across bayer quads
		// First a horizontal straight line across the array
		dataBuffer->markSegmentStart(kLeftHorizonalSegment);
		for (col = 0; col < pixBuffer.fArea.W()>>1; col += 2) {
			dataBuffer->addQuad(pixBuffer, (pixBuffer.fArea.H() >> 1), col, colorToPlane);		
		}
		dataBuffer->markSegmentStart(kRightHorizonalSegment);
		for (; col < pixBuffer.fArea.W()-1; col += 2) {
			dataBuffer->addQuad(pixBuffer, (pixBuffer.fArea.H() >> 1), col, colorToPlane);		
		}
		// Then a vertical straight line across the array
		dataBuffer->markSegmentStart(kTopVerticalSegment);
		for (row = 0; row < pixBuffer.fArea.H()>>1; row += 2) {
			dataBuffer->addQuad(pixBuffer, row, (pixBuffer.fArea.W() >> 1), colorToPlane);		
		}
		dataBuffer->markSegmentStart(kBottomVerticalSegment);
		for (; row < pixBuffer.fArea.H()-1; row += 2) {
			dataBuffer->addQuad(pixBuffer, row, (pixBuffer.fArea.W() >> 1), colorToPlane);		
		}		
		
		real32 r = ((real32) pixBuffer.fArea.W())/((real32) pixBuffer.fArea.H());
		
		// Then a diagonal across the array
		dataBuffer->markSegmentStart(kTopLeftDiagonalSegment);
		for (row = 0; row < (pixBuffer.fArea.H()>>1); row += 2) {
			col = (uint32) (((real32) row) * r);
			dataBuffer->addQuad(pixBuffer, row, col, colorToPlane);		
		}
		dataBuffer->markSegmentStart(kBottomRightDiagonalSegment);
		for (; row < pixBuffer.fArea.H()-1; row += 2) {
			col = (uint32) (((real32) row) * r);
			dataBuffer->addQuad(pixBuffer, row, col, colorToPlane);		
		}
		
		// And a diagonal the other way
		dataBuffer->markSegmentStart(kTopRightDiagonalSegment);
		for (row = 0; row < (pixBuffer.fArea.H()>>1); row += 2) {
			col = pixBuffer.fArea.W() - ((uint32) (((real32) row) * r));
			dataBuffer->addQuad(pixBuffer, row, col, colorToPlane);		
		}
		dataBuffer->markSegmentStart(kBottomLeftDiagonalSegment);
		for (; row < pixBuffer.fArea.H()-1; row += 2) {
			col = pixBuffer.fArea.W() - ((uint32) (((real32) row) * r));
			dataBuffer->addQuad(pixBuffer, row, col, colorToPlane);		
		}
	}
}

real32 M8_devignette::computeOffsetVal(uint32 segment, uint32 color, int32 offset)
{
	real64 accum = 0.0F;
	uint32 count = 0;
	int32 center = ((int32) ((dataBuffer->getSegmentLength(segment) + dataBuffer->getSegmentLength(segment+1))>>1)) + offset;
	for (int32 loc = 0; ((center + loc) < ((int32) (dataBuffer->getSegmentLength(segment) + dataBuffer->getSegmentLength(segment+1))) 
						 && ((center - loc) >=0)); loc++) {
		accum += pow(((real64) dataBuffer->getData(color, dataBuffer->getSegmentStart(segment)+center+loc)) 
					 - ((real64) dataBuffer->getData(color, dataBuffer->getSegmentStart(segment)+center-loc)), 
					 2.0);
		count++;
	}	
	return ((real32) accum)/((real32) count);				 
}


bool M8_devignette::AnalyseStage2(bool multiple)

{
#if _DEBUG
	FILE *dumpThree;
#if qWinOS
	fopen_s( &dumpThree, "/Users/sandy/Desktop/ThreeD.txt", "w" );
#else
	dumpThree = fopen("/Users/sandy/Desktop/ThreeD.txt", "w" );
#endif
	dump3D(dumpThree);
	fclose( dumpThree );
#endif
	
	if (pixBuffer.fData != NULL) {
		bufferSat = false;
		//Set all the buffers we'll need
		this->BuildDataBuffer();
		
		// Get the fitter initialised
		lm_control_type control;
		dng_lmfit test;
		test.lm_initialize_control( &control );
		
		// First find the optical center of the sensor.....
		// Note optical center here is the "center of mass" of the vignetting curve,
		// not the real optical center
		
		bool foundCenter = true;
		
		if (doOffsets) {
			for (uint32 segment = kLeftHorizonalSegment; segment <= kTopVerticalSegment; segment+=2) 
			{
				int32 center = 0;
				uint32 length = dataBuffer->getSegmentLength(segment)+dataBuffer->getSegmentLength(segment+1);
				real64 *pixelCoords = new real64[length];
				real64 *pixelVals = new real64[length];
				
				for (uint32 color = 0; color < kbayerColors; color++) {
					// Load the data.......
					int32 sensorCenter = 0;
					uint32 index = 0;
					for (uint32 dataSegment = segment; dataSegment <= segment+1; dataSegment++) {
						for (uint32 i = 0; i < dataBuffer->getSegmentLength(dataSegment); i++) {
							switch (dataSegment) {
								case kLeftHorizonalSegment:
								case kRightHorizonalSegment:
									pixelCoords[index] = (real64) dataBuffer->getCol(color, dataBuffer->getSegmentStart(dataSegment)+i);
									sensorCenter = pixBuffer.fArea.W()>>1;
									break;
								case kTopVerticalSegment:
								case kBottomVerticalSegment:
									pixelCoords[index] = (real64) dataBuffer->getRow(color, dataBuffer->getSegmentStart(dataSegment)+i);
									sensorCenter = pixBuffer.fArea.H()>>1;
									break;
								default:
									break;
							}
							pixelVals[index++] = (real64) dataBuffer->getData(color, dataBuffer->getSegmentStart(dataSegment)+i);
						}
					}
					
					dng_local_printf::printf( "Analysing center on color %d: \n", color);
					// Analyse the pixel vales versus linear coordinate
					function[segment][color].user_t = pixelCoords;
					function[segment][color].user_y = pixelVals;
					function[segment][color].setEquationType(kLensCenterEquationType);
					function[segment][color].setAsymetry(1.0);
					
					// perform the fit:
					test.lm_minimize( length, 
									 function[segment][color].getParNumber(), 
									 function[segment][color].getP(),
									 &function[segment][color], 
									 &control );
	#if kLensCenterEquationType == 7
					// For a type 7 equation, which is symetrical anyway, we can just use
					// the peak
					center = ((int32) (-function[segment][color].getP()[1]/(2.0*function[segment][color].getP()[2]))) - sensorCenter;
	#else				
					// Now we brute force find the best left-right match, rather than the peak. We can't
					// use the peak because higher order equations aren't symetrical
					#define N 100
					bool lMoved = false;
					bool rMoved = false;
					int32 lAnchor = N;
					int32 rAnchor = length - N - 1;
					int32 maxIterations = length/3;
					do {
						if (function[segment][color].evalSelected(pixelCoords[lAnchor]) < function[segment][color].evalSelected(pixelCoords[rAnchor])) {
							lAnchor++;
							lMoved = true;
						}
						else {
							rAnchor--;
							rMoved = true;
						}
						maxIterations--;
					} while ((!lMoved || !rMoved) && (maxIterations > 0));
					
					// Now tighten up the initial estimate with a least errors method.....
					int32 currentLoc = (lAnchor + rAnchor)>>1;
					bool directionIsLeft = false;
					bool directionIsRight = false;
					do {
						real32 left = computeOffsetVal(segment, color, currentLoc - (length >> 1) - 1);
						real32 right = computeOffsetVal(segment, color, currentLoc - (length >> 1) + 1);
						if (left < right) {
							currentLoc--;
							directionIsLeft = true;
						}
						else {
							currentLoc++;
							directionIsRight = true;
						}						
					} while (!(directionIsLeft && directionIsLeft) && (currentLoc > ((int32) length/4)) && (currentLoc < ((int32) length/4*3)));				
					
					currentLoc = ((int32) pixelCoords[currentLoc]) - sensorCenter;

#if _DEBUG					
					real32 min = computeOffsetVal(segment, color, 0);
					int32 minLoc = 0;
					real32 val;
					for (int32 i = -500; i < +500; i++) {
						if ((val = computeOffsetVal(segment, color, i)) < min) {
							minLoc = i;
							min = val;
						}
//						printf("%d %f\n", i, val);
					}
#endif
					
					// Check that we reached a sane number, else just use zero.....
					if (maxIterations <= 0) {
						foundCenter = false;
						center = 0;
					}
					else {
						center = ((int32) pixelCoords[(lAnchor + rAnchor)>>1]) - sensorCenter;
					}
#if _DEBUG						
					printf("center: %d current loc: %d minLoc: %d\n", center, currentLoc, minLoc);
#endif
					center = currentLoc;
#endif				
					for (uint32 writeSegment = kLeftHorizonalSegment; writeSegment < kMaxSegments; writeSegment++) 
					{
						switch (segment) {
							case kLeftHorizonalSegment:
							case kRightHorizonalSegment:
								function[writeSegment][color].setCenterX(center);
								break;
							case kTopVerticalSegment:
							case kBottomVerticalSegment:
								function[writeSegment][color].setCenterY(center);
								break;
							default:
								break;
						}
					}
				}
				if (pixelCoords != NULL) delete []pixelCoords;
				if (pixelVals != NULL) delete []pixelVals;
			} // Done with the vertical and horizontal segments......
		}
		else {
			foundCenter = false;
		}
		
		if (!foundCenter) {
			for (uint32 color = 0; color < kbayerColors; color++) {
				for (uint32 writeSegment = kLeftHorizonalSegment; writeSegment < kMaxSegments; writeSegment++) 
				{
					function[writeSegment][color].setCenterX(0);
					function[writeSegment][color].setCenterY(0);
				}
			}
		}
		
		uint32 startSeg = 0;
		uint32 endSeg = 0;
		switch (numSegments) {
			case 1:
				startSeg = kTopLeftDiagonalSegment;
				endSeg = kTopLeftDiagonalSegment;
				break;
			case 4:
				startSeg = kTopLeftDiagonalSegment;
				endSeg = kBottomLeftDiagonalSegment;
				break;
			case 8:
				startSeg = kLeftHorizonalSegment;
				endSeg = kBottomLeftDiagonalSegment;
				break;
			default:
				break;
		}
		
		// Now find the actual vignetting curve equations
		for (uint32 segment = startSeg; segment <= endSeg; segment++) 
		{
			uint32 dataSegment = (numSegments != 1) ? segment : kAllSegments;
			for (uint32 case_p = 0; case_p < kbayerColors; case_p++)
			{
				if (factorBuffer[case_p]) delete []factorBuffer[case_p];
				factorBuffer[case_p] = new real64[dataBuffer->elementsIn()];
				if (radius[case_p]) delete []radius[case_p];
				radius[case_p] = new real64[dataBuffer->elementsIn()];
			}
			illumBuffer->clearComp();

			// Now we have the parameters of a correction curve..........

			for (uint32 color = 0; color < kbayerColors; color++) {
				for (uint32 i = 0; i < dataBuffer->getSegmentLength(dataSegment); i++) {
					// Find out the row and column of the data point, then get the K factor for the color we're working on
					factorBuffer[color][i] = illumBuffer->getK(dataBuffer->getRow(0, dataBuffer->getSegmentStart(dataSegment)+i), 
														dataBuffer->getCol(0, dataBuffer->getSegmentStart(dataSegment)+i), color);
				}
			}
			// factorBuffer is what we fit to
			for (uint32 color = 0; color < kbayerColors; color++) {
				for (uint32 i = 0; i < dataBuffer->getSegmentLength(dataSegment); i++) {
					// factorBuffer is the pixel value divided by the maximum value for that color,
					// divided by the K factor from the illumination comp system (if enabled)
					factorBuffer[color][i] = 1.0;
					real64 r = ((real64) dataBuffer->getData(color, dataBuffer->getSegmentStart(dataSegment)+i))/
																	((real64) dataBuffer->getBufferMax(color));
					
					if ((function[segment][color].getCorrectionType() == differentialCorrection) && (color != referenceBayerColor)) {
						factorBuffer[color][i] = ((real64) dataBuffer->getData(referenceBayerColor, dataBuffer->getSegmentStart(dataSegment)+i))/
						((real64) dataBuffer->getBufferMax(referenceBayerColor))/r;
					}
					else {
						factorBuffer[color][i] = r/factorBuffer[color][i];
					}
				}
			
				// and at the same time, we will need radius factors....
				// which is what we fit against
				for (uint32 i = 0; i < dataBuffer->getSegmentLength(dataSegment); i++) {
					real32 r;
					
					r = (real32) pow(pow((real64) ((int32) dataBuffer->getRow(0, dataBuffer->getSegmentStart(dataSegment)+i)) - ((int32)pixBuffer.fArea.H()>>1) - function[segment][color].getCenterY(),2)
						+ pow((real64) ((int32) dataBuffer->getCol(0, dataBuffer->getSegmentStart(dataSegment)+i)) - ((int32)pixBuffer.fArea.W()>>1) - function[segment][color].getCenterX(),2), 0.5);
					radius[color][i] = r;

					if (((int32) dataBuffer->getCol(0, dataBuffer->getSegmentStart(dataSegment)+i)) - ((int32)pixBuffer.fArea.W()>>1) < 0) {
						radius[color][i] = -radius[color][i];
					}

				}
/*				
				for (uint32 i = 0; i < dataBuffer->getSegmentLength(dataSegment); i++) {
					printf("%d %d %d %d %d %d %d %f  %f\n", 
						   segment, 
						   dataSegment, 
						   color, 
						   i, 
						   dataBuffer->getRow(color, dataBuffer->getSegmentStart(dataSegment)+i), 
						   dataBuffer->getCol(color, dataBuffer->getSegmentStart(dataSegment)+i), 
						   dataBuffer->getData(color, dataBuffer->getSegmentStart(dataSegment)+i), 
						   (float) radius[color][i], 
						   (float) factorBuffer[color][i]);
					}
*/				 
			}

			// That gives us an array of vignette factors we can do a curve fit against:
			// auxiliary settings:

			for (uint32 color = 0; color <kbayerColors; color++) {

				dng_local_printf::printf( "Analysing segment %d color %d:\n", segment, color);
				// Analyse the factor versus radius
				function[segment][color].user_t = radius[color];
				function[segment][color].user_y = factorBuffer[color];

				// If we're doing multiple equations, we want to track the norms to select
				// the best one
				real64 norms[function[segment][color].kmaxEquations];
				for (uint32 eq = 0; eq < function[segment][color].kmaxEquations; eq++) {
					norms[eq] = 9999999;
				}

				real64 minNorm = 9999999;

				uint32 firstEq = 0;
				uint32 lastEq = function[segment][color].getNumEquations();

				// If we only a single equation, we want it to be 2
				if (!multiple) {
					firstEq = 2;
					lastEq = firstEq +1;
				}

				for (uint32 eq = firstEq; eq < lastEq; eq++) {
					function[segment][color].setEquationType(eq);
					function[segment][color].setAsymetry(asymetry);

					// perform the fit:

					test.lm_minimize( dataBuffer->getSegmentLength(dataSegment), 
									 function[segment][color].getParNumber(), 
									 function[segment][color].getP(),
									 &function[segment][color], &control );

					// print results:

#if qDNGValidate
					if (gVerbose)
					{
						dng_local_printf::printf( "\nstatus: %s after %d evaluations\n", test.lm_get_shortmsg(control.info), control.nfev );
					}
#endif
					norms[eq] = control.fnorm;
					if (minNorm > norms[eq]) {
						minNorm = norms[eq];
					}
					dng_local_printf::printf( "Equation %d norm: %le\n", eq, norms[eq]);			
				}

				int32 eqSelect = -1;
				for (uint32 eq = 0; eq < function[segment][color].getNumEquations(); eq++) {
					if ((minNorm*1.02 > norms[eq]) && (eqSelect < 0)) {
						eqSelect = eq;
						function[segment][color].setEquationType(eq);
						function[segment][color].normalise();
					}
				}
				dng_local_printf::printf( "Equation %d selected - parameters:\n", eqSelect);
				dng_local_printf::printf( "%d ", function[segment][color].getCenterX());
				dng_local_printf::printf( "%d ", function[segment][color].getCenterY());
				for (uint32 p = 0; p < function[segment][color].getParNumber(); p++) {
					dng_local_printf::printf( "%le ", function[segment][color].getP()[p]);
				}
				dng_local_printf::printf( "\n");
				dng_local_printf::printf( "\n");
			}
		}
	}

	for (uint32 color = 0; color <kbayerColors; color++) {
		if (dataBuffer->getBufferMax(color) >0xfdff) {
			bufferSat = true;
		}
	}
	return bufferSat;
  }



void M8_devignette::PrintAnalysisData(FILE *stream)
{

	for (uint32 i = 0; i < dataBuffer->getDataLength(0); i++) {
		fprintf(stream, "\n%u, %u", dataBuffer->getRow(0, i), dataBuffer->getCol(0, i));
		for (uint32 color = 0; color < kbayerColors; color++) {
			fprintf(stream, ", %u", dataBuffer->getData(color, i));
		}
		for (uint32 color = 0; color < kbayerColors; color++) {
			fprintf(stream, ", %le", illumBuffer->getK(dataBuffer->getRow(color, i), dataBuffer->getCol(color, i), color));
		}
		fprintf(stream, ", %p", radius[i]);
		for (uint32 color = 0; color < kbayerColors; color++) {
			fprintf(stream, ", %le", factorBuffer[color][i]);
		}
		for (uint32 color = 0; color < kbayerColors; color++) {
			fprintf(stream, ", %le", function[0][color].evalSelected(radius[color][i]));
		}

	}

}


void M8_devignette::OutputProfile(FILE *stream)
{
	fprintf(stream, "profile_version 1\n");
	fprintf(stream, "cornerfix_version ");
	fprintf(stream, CornerFixVersion);
	fprintf(stream, "\n");
	fprintf(stream, "description \"\"\n");
	fprintf(stream, "creator \"\"\n");
	fprintf(stream, "copyright \"\"\n");
	fprintf(stream, "licence \"This work is licensed under the Creative \"\n");
	fprintf(stream, "licence \"Commons Attribution 3.0 License. To view a copy of this license, \"\n");
	fprintf(stream, "licence \"visit http://creativecommons.org/licenses/by/3.0/ or send a letter \"\n");
	fprintf(stream, "licence \"to Creative Commons, 171 Second Street, Suite 300, San Francisco, \"\n");
	fprintf(stream, "licence \"California, 94105, USA.\"\n");

	uint32 startSeg = 0;
	uint32 endSeg = 0;
	switch (numSegments) {
		case 1:
			startSeg = kTopLeftDiagonalSegment;
			endSeg = kTopLeftDiagonalSegment;
			break;
		case 4:
			startSeg = kTopLeftDiagonalSegment;
			endSeg = kBottomLeftDiagonalSegment;
			break;
		case 8:
			startSeg = kLeftHorizonalSegment;
			endSeg = kBottomLeftDiagonalSegment;
			break;
		default:
			break;
	}

	for (uint32 color = 0; color < kbayerColors; color++) {
		for (uint32 writeSegment = startSeg; writeSegment <= endSeg; writeSegment++) 
		{
			switch (writeSegment) {
				case kTopLeftDiagonalSegment:
					fprintf(stream, "color %u equation ", color);
					break;
				case kBottomRightDiagonalSegment:
					fprintf(stream, "BottomRight %u equation ", color);
					break;
				case kBottomLeftDiagonalSegment:
					fprintf(stream, "BottomLeft %u equation ", color);
					break;
				case kTopRightDiagonalSegment:
					fprintf(stream, "TopRight %u equation ", color);
					break;
				case kLeftHorizonalSegment:
					fprintf(stream, "LeftHorizonal %u equation ", color);
					break;
				case kRightHorizonalSegment:
					fprintf(stream, "RightHorizonal %u equation ", color);
					break;
				case kTopVerticalSegment:
					fprintf(stream, "TopVertical %u equation ", color);
					break;
				case kBottomVerticalSegment:
					fprintf(stream, "BottomVertical %u equation ", color);
					break;
				default:
					break;
			}

			fprintf(stream, "%u ", function[writeSegment][color].getEquationType());
			for (uint32 p = 0; p < function[writeSegment][color].getParNumber(); p++) {
				if (function[writeSegment][color].isPolynomial()) {
					fprintf(stream, "%le ", function[writeSegment][color].getP()[p]/function[writeSegment][color].getP()[0]);
					}
				else {
					fprintf(stream, "%le ", function[writeSegment][color].getP()[p]);
				}
			}
			fprintf(stream, "\n");
		}
		fprintf(stream, "center %u location %d %d ", color, function[kTopLeftDiagonalSegment][color].getCenterX(), 
				function[kTopLeftDiagonalSegment][color].getCenterY());
		fprintf(stream, "\n");
	}

	fprintf(stream, "end\n");

}

void M8_devignette::ReadEquationLine(FILE *stream, uint32 segmentSel)
{
	uint32 color = 0;
	uint32 eq_type = 0;
#if qWinOS
	fscanf_s(stream, "%ld equation %ld ", &color, &eq_type);
#else
	fscanf(stream, "%u equation %u ", &color, &eq_type);
#endif
	function[segmentSel][color].setEquationType(eq_type);
	for (uint32 p = 0; p < function[segmentSel][color].getParNumber(); p++) {
#if qWinOS
		fscanf_s(stream, "%le ", &(function[segmentSel][color].getP()[p]));
#else
		fscanf(stream, "%le ", &(function[segmentSel][color].getP()[p]));
#endif
	}
#if qWinOS
	fscanf_s(stream, "\n");
#else
	fscanf(stream, "\n");
#endif
}

bool M8_devignette::ReadProfile(FILE *stream)
{
	uint32 version = 0;
	dng_file_string lineName;
	bool equationsRead = false;
	numSegments = 1;
	BayerGreenSplit = kMaxGreenSplit;
	
	for (uint32 color = 0; color < kbayerColors; color++) {
		for (uint32 writeSegment = kLeftHorizonalSegment; writeSegment < kMaxSegments; writeSegment++) 
		{
			function[writeSegment][color].setCenterX(0);
			function[writeSegment][color].setCenterY(0);
		}
	}
	
   // Cycle until end of file reached:
   while( !feof( stream ) )
   {
		lineName.read_string(stream);
		if (lineName.Matches ("profile_version", true))
		{
#if qWinOS
			fscanf_s(stream, "%ld", &version);
			fscanf_s(stream, "\n");
#else
			fscanf(stream, "%u", &version);
			fscanf(stream, "\n");
#endif
		}
		else if (lineName.Matches ("description", true))
		{
#if qWinOS
			fscanf_s(stream, "\n");
#else
			fscanf(stream, "\n");
#endif		
		}
		else if (lineName.Matches ("color", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kTopLeftDiagonalSegment);
		}
		else if (lineName.Matches ("BottomLeft", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kBottomLeftDiagonalSegment);
			if (numSegments < 4) numSegments = 4;
		}
		else if (lineName.Matches ("BottomRight", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kBottomRightDiagonalSegment);
			if (numSegments < 4) numSegments = 4;
		}
		else if (lineName.Matches ("TopRight", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kTopRightDiagonalSegment);
			if (numSegments < 4) numSegments = 4;
		}
		else if (lineName.Matches ("LeftHorizonal", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kLeftHorizonalSegment);
			numSegments = 8;
		}
		else if (lineName.Matches ("RightHorizonal", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kRightHorizonalSegment);
			numSegments = 8;
		}
		else if (lineName.Matches ("TopVertical", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kTopVerticalSegment);
			numSegments = 8;
		}
		else if (lineName.Matches ("BottomVertical", true))
		{
			if (version > 0) equationsRead = true;
			ReadEquationLine(stream, kBottomVerticalSegment);
			numSegments = 8;
		}
		else if (lineName.Matches ("center", true))
		{
			uint32 color = 0;
			int32 X = 0;
			int32 Y = 0;
#if qWinOS
			fscanf_s(stream, "%ld location %ld %ld ", &color, &X, &Y);
#else
			fscanf(stream, "%u location %d %d ", &color, &X, &Y);
#endif
			for (uint32 writeSegment = kLeftHorizonalSegment; writeSegment < kMaxSegments; writeSegment++) 
			{
				function[writeSegment][color].setCenterX(X);
				function[writeSegment][color].setCenterY(Y);
			}
#if qWinOS
			fscanf_s(stream, "\n");
#else
			fscanf(stream, "\n");
#endif
		}
		else if (lineName.Matches ("BayerGreenSplit", true))
		{
#if qWinOS
			fscanf_s(stream, "%ld \n", &BayerGreenSplit);
#else
			fscanf(stream, "%u \n", &BayerGreenSplit);
#endif
		}
		else
		{
#if qWinOS
			fscanf_s(stream, "\n");
#else
			fscanf(stream, "\n");
#endif
		}
   }

   return equationsRead;

}


void M8_devignette::dump3D(FILE *stream)
{
	
	int dataSize = 0;
	uint32 xN = 100;
	uint32 yN = 100;
	
	// Initialise the databuffer for a grid array of analysis data
	if (pixBuffer.fData != NULL) 
	{
		dataSize = (xN + 1) * (yN + 1);
		// and then horizontal and vertical
	}
	
	int colorToPlane[kbayerColors];
	
	if (pixBuffer.Planes() > 1) {
		// This has to be an 3 plane RGB layout
		colorToPlane[color_red] = 0;
		colorToPlane[color_green1] = 1;
		colorToPlane[color_green2] = 1;
		colorToPlane[color_blue] = 2;
	}
	else {
		// Must be a single plane CFA layout
		colorToPlane[color_red] = 0;
		colorToPlane[color_green1] = 0;
		colorToPlane[color_green2] = 0;
		colorToPlane[color_blue] = 0;		
	}
	
	DataStore *gridBuffer = new DataStore(pixBuffer);

	if (gridBuffer) {
		
		if ((pixBuffer.fData != NULL) && (gridBuffer->getDataLength(0) < 1)) {
			uint32 row = 0, col = 0;
			uint32 xCount=0, yCount=0;
			gridBuffer->clearBuffer(pixBuffer, dataSize);
			for (xCount = 0; xCount < xN; xCount++) {
				for (yCount = 0; yCount < yN; yCount++) {
					gridBuffer->addQuad(pixBuffer, row, col, colorToPlane);
					row+= pixBuffer.fArea.H()/yN;
				}
				col+= pixBuffer.fArea.W()/xN;
			}
		}
		
		for (uint32 color = 0; color < kbayerColors; color++) {
			for (uint32 i = 0; i < gridBuffer->getDataLength(0); i++) {
				fprintf(stream, 
						"\n%u  %u", 
						gridBuffer->getCol(0, i) - ((int32)pixBuffer.fArea.W()>>1) - function[2][color].getCenterX(), 
						gridBuffer->getRow(0, i) - ((int32)pixBuffer.fArea.H()>>1) - function[2][color].getCenterY());
				fprintf(stream, 
						"  %u", 
						gridBuffer->getData(color, i));
			}
			fprintf(stream, "\n\n");
		}
		
		delete gridBuffer;
	}
}

void M8_devignette::test_fit()

{
    int m_dat = 15;
    int n_p =  3;

    real64 t[15] = { .07, .13, .19, .26, .32, .38, .44, .51,
                     .57, .63, .69, .76, .82, .88, .94 };
    real64 y[15] = { .24, .35, .43, .49, .55, .61, .66, .71,
                     .75, .79, .83, .87, .90, .94, .97 };
    real64 p[3] = { 1., 1., 1. }; // use any starting value, but not { 0,0,0 }

    // auxiliary settings:
 
    lm_control_type control;
    dng_lmfit_functions data;

	dng_lmfit test;

    test.lm_initialize_control( &control );

    data.user_t = t;
    data.user_y = y;

    // perform the fit:

    dng_local_printf::printf( "modify or replace lm_print_default for less verbous fitting\n");

    test.lm_minimize( m_dat, n_p, p,
                 &data, &control );

    // print results:

    dng_local_printf::printf( "status: %s after %d evaluations\n",
            test.lm_get_shortmsg(control.info), control.nfev );

}

uint32 M8_devignette::getBayerGreenSplit()
{
	return BayerGreenSplit;
}


