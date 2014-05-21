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
 * M8_devignette.h
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
 * 18 Jun 2007 : 0.9.0.5
 * 22 Jun 2007 : 0.9.0.6
 * 1 Jul 2007 : 0.9.1.0
 * 1 Sept 2007 : 0.9.1.0 Mac version
 * 12 Sept 2009 : 1.2.0.0
 * 14 Oct 2009 : 1.3.0.0
 * 9 Nov 2009 : 1.3.0.3
 * 6 October 2010 : 1.4.0.0
 * 22 October 2010 : 1.4.0.2
 * 4 December 2013 : 1.4.2.6
 *
 */

/*
 * Handles reference image analysis, and 
 * devignetting of images
 */

#ifndef __M8_devignette__
#define __M8_devignette__


/*****************************************************************************/

#include "dng_auto_ptr.h"
#include "dng_classes.h"
#include "dng_memory.h"
#include "dng_rational.h"
#include "dng_rect.h"
#include "dng_sdk_limits.h"
#include "dng_pixel_buffer.h"
#include "dng_file_stream.h"
#include "dng_lmfit_functions.h"
#include "CornerFixVersion.h"
#include "dng_line_processor.h"
#include "dng_simple_image.h"

/*****************************************************************************/

enum localColors {
	color_red = 0,
	color_green1,
	color_green2,
	color_blue
};


enum GreenSplitLimits {
	kMaxGreenSplit = 9999
};

//////////////////////////////////////////////////////////////
//
// Array templates
//
//////////////////////////////////////////////////////////////


// Checks on array subscripts
//#define arrayCheck

template <class T> class Array1D 
{
public:
    Array1D( void )
	{
		arrayLoc = NULL;
	}
	~Array1D( void )
	{
		if (arrayLoc != NULL)
		{
			delete []arrayLoc;
		}
	}
	void setSize(int x)
	{
		int arraySize = x;
		if ((arrayLoc != NULL) && (arraySize > xSize))
		{
			delete []arrayLoc;
			arrayLoc = NULL;
		}
		if (arrayLoc == NULL)
		{
			try 
			{
				arrayLoc = new T[arraySize];
				xSize = x;
			}
			catch (...) {
				arrayLoc = NULL;
				xSize = 0;
			}
		}
	}
	void set(int x, T val)
	{
#ifdef arrayCheck
		if (x < xSize){
#endif
			arrayLoc[x] = val;
#ifdef arrayCheck
		}
		else {
			dng_local_printf::printf("\n************* Array Check Error - %d", x );
		}
#endif
	}
	T get(int x)
	{
		return arrayLoc[x];
	}
	int elementsIn(void){
		return xSize;
	}
	int getXSize(void)
	{
		return xSize;
	}
private:
    T *arrayLoc;
    int xSize;
};

template <class T> class Array2D 
{
public:
    Array2D( void )
	{
		arrayLoc = NULL;
	}
	~Array2D( void )
	{
		if (arrayLoc != NULL)
		{
			delete []arrayLoc;
		}
	}
	void setSize(int x, int y)
	{
		int arraySize = x*y;
		if ((arrayLoc != NULL) && (arraySize > xSize*ySize))
		{
			delete []arrayLoc;
			arrayLoc = NULL;
		}
		if (arrayLoc == NULL)
		{
			try 
			{
				arrayLoc = new T[arraySize];
				xSize = x;
				ySize = y;
			}
			catch (...) {
				arrayLoc = NULL;
				xSize = 0;
				ySize = 0;
			}
		}
	}
	void set(int x, int y, T val)
	{
#ifdef arrayCheck
		if ((x < xSize) && (y < ySize)){
#endif
			arrayLoc[x+y*xSize] = val;
#ifdef arrayCheck
		}
		else {
			dng_local_printf::printf("\n************* Array Check Error - %d, %d", x, y);
		}
#endif
	}
	T get(int x, int y)
	{
		return arrayLoc[x+y*xSize];
	}
	int elementsIn(void){
		return xSize*ySize;
	}
	int getXSize(void)
	{
		return xSize;
	}
	int getYSize(void)
	{
		return ySize;
	}
private:
    T *arrayLoc;
    int xSize;
	int ySize;
};

template <class T> class Array3D 
{
public:
    Array3D( void )
	{
		arrayLoc = NULL;
	}
	~Array3D( void )
	{
		if (arrayLoc != NULL)
		{
			delete []arrayLoc;
		}
	}
	void setSize(int x, int y, int z)
	{
		int arraySize = x*y*z;
		if ((arrayLoc != NULL) && (arraySize > xSize*ySize*zSize))
		{
			delete []arrayLoc;
			arrayLoc = NULL;
		}
		if (arrayLoc == NULL)
		{
			try 
			{
				arrayLoc = new T[arraySize];
				xSize = x;
				ySize = y;
				zSize = z;
			}
			catch (...) {
				arrayLoc = NULL;
				xSize = 0;
				ySize = 0;
				zSize = 0;
			}
		}
	}
	void set(int x, int y, int z, T val)
	{
#ifdef arrayCheck
		if ((x < xSize) && (y < ySize) && (z < zSize)){
#endif
			arrayLoc[x+y*xSize+z*xSize*ySize] = val;
#ifdef arrayCheck
		}
		else {
			dng_local_printf::printf("\n************* Array Check Error - %d, %d, %d", x, y, z);
		}
#endif
	}
	T get(int x, int y, int z)
	{
		return arrayLoc[x+y*xSize+z*xSize*ySize];
	}
	int elementsIn(void){
		return xSize*ySize*zSize;
	}
	int getXSize(void)
	{
		return xSize;
	}
	int getYSize(void)
	{
		return ySize;
	}
	int getZSize(void)
	{
		return zSize;
	}
private:
    T *arrayLoc;
    int xSize;
	int ySize;
	int zSize;
};

//////////////////////////////////////////////////////////////
//
// The data store for the vignetting data
//
//////////////////////////////////////////////////////////////


class DataStore
{
	
public:
	
	
private:
	
	
	Array3D<uint32> dataArray;
	
	uint32 dataLength[kbayerColors];
	uint32 bufferMax[kbayerColors];
	uint32 segmentLength[kMaxSegments];
    uint32 cropOrigHi, cropOrigVi, cropWidthi, cropHeighti;
	
public:
	
	DataStore (dng_pixel_buffer &pixBuffer);
	
	virtual ~DataStore ();
	
	void clearBuffer(dng_pixel_buffer &pixBuffer);
	void clearBuffer(dng_pixel_buffer &pixBuffer, int dataSize);
	int elementsIn(void);
	
	void addData(uint32 color, uint32 row, uint32 col, uint32 data);
	void addQuad(dng_pixel_buffer &pixBuffer, uint32 row, uint32 col, int colorToPlane[]);
	uint32 getDataLength(uint32 color);
	uint32 getRow(uint32 color, uint32 index);
	uint32 getCol(uint32 color, uint32 index);
	uint32 getData(uint32 color, uint32 index);
	real64 getCompData(uint32 color, uint32 index);
	int32 getBufferMax(uint32 color);
	void markSegmentStart(uint32 segmentSelect);
	uint32 getSegmentStart(uint32 segmentSelect);
	uint32 getSegmentLength(uint32 segmentSelect);
    void setCropArea(real64 originH, real64 originV, real64 width, real64 height);
	
};

//////////////////////////////////////////////////////////////
//
// The illuminant compensation class
//
//////////////////////////////////////////////////////////////

class IllumComp
{
private:
	
	real64 mRow[kbayerColors];
	real64 mCol[kbayerColors];
	real64 bRow[kbayerColors];
	real64 bCol[kbayerColors];
	real64 bBoth[kbayerColors];
	real64 maxPixelval[kbayerColors];
	real64 kmax[kbayerColors];
	
public:
	IllumComp ();
	
	virtual ~IllumComp ();
	
	void clearComp();
	real64 getmRow(uint32 color); 
	real64 getmCol(uint32 color); 
	real64 getbRow(uint32 color); 
	real64 getbCol(uint32 color); 
	real64 getbBoth(uint32 color); 
	void setmRow(uint32 color, real64 val); 
	void setmCol(uint32 color, real64 val); 
	void setbRow(uint32 color, real64 val); 
	void setbCol(uint32 color, real64 val); 
	void setbBoth(uint32 color, real64 val); 
	real64 getmaxPixelval(uint32 color); 
	void setmaxPixelval(uint32 color, real64 val); 
	real64 getK(uint32 row, uint32 col, uint32 color);

};

//////////////////////////////////////////////////////////////
//
// The master vignetting analysis class
//
//////////////////////////////////////////////////////////////

class M8_devignette
{
public:


private:

	virtual void BuildDataBuffer();
	bool bufferSat;
	int antiAliasStrength;
	real64 asymetry;
	real64 *factorBuffer[kbayerColors];
	real64 *radius[kbayerColors];
	dng_lmfit_functions function[kMaxSegments][kbayerColors];
	dng_pixel_buffer pixBuffer;
	DataStore * dataBuffer;
	IllumComp * illumBuffer;
	bool doOffsets;
	uint32 numSegments;
	uint32 BayerGreenSplit;
    real64 cropOrigH, cropOrigV, cropWidth, cropHeight;

public:
	M8_devignette(void);
	~M8_devignette(void);

	virtual void SetAntiAlias(int strength);
	
	virtual int GetAntiAlias(void);

	virtual void SetAsymetry(real64 strength);
	
	virtual real64 GetAsymetry(void);
	
	dng_lmfit_functions *getFitFunctions(int segment);
	
	virtual void SetStage2Buffer (const dng_pixel_buffer * buffer);
	
	virtual void SetStage2Buffer (dng_simple_image * image);

	virtual void correctIluminant();

	virtual bool AnalyseStage2(bool multiple);

	virtual void PrintAnalysisData(FILE *stream);

	virtual void OutputProfile(FILE *stream);
	
	virtual void ReadEquationLine(FILE *stream, uint32 segmentSel);

	virtual bool ReadProfile(FILE *stream);

	virtual void test_fit();
	
	virtual void dump3D(FILE *stream);
	
	virtual uint32 getNumSegments(void);

	virtual void setNumSegments(uint32 val);
	
	virtual uint32 getBayerGreenSplit();
    
    virtual void setCropArea(real64 originH, real64 originV, real64 width, real64 height);
	
private:
	
	real64 getVignetteFactor(uint32 index);
	
	real32 computeOffsetVal(uint32 segment, uint32 color, int32 offset);

};

/*****************************************************************************/

#endif
	
/*****************************************************************************/
