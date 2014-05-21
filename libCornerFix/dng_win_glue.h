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
 * dng_win_glue.h
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
 * 23 Oct 2007 : 0.9.1.5
 * 29 Nov 2007 : 0.9.1.7
 * 20 Dec 2007 : 0.9.1.8
 * 12 Sept 2009 : 1.2.0.0;
 * 31 July 2012 : 1.4.2.2
 *
 */

/*
 * This is the primary point of conversion from Windows
 * land into DNG land, and manages a managed/unmanaged code
 * transition.......
 * This based on T. Knoll's original dng_validate code
 */

#pragma once

#include "dng_color_space.h"
#include "dng_exceptions.h"
#include "dng_file_stream.h"
#include "dng_globals.h"
#include "dng_threaded_host.h"
#include "dng_ifd.h"
#include "dng_image_writer.h"
#include "dng_info.h"
#include "dng_highres_info.h"
#include "dng_mosaic_info.h"
#include "dng_negative.h"
#include "dng_render.h"
#include "dng_simple_image.h"
#include "dng_tag_codes.h"
#include "dng_tag_types.h"
#include "dng_tag_values.h"
#include "dng_xmp_sdk.h"
#include "dng_pixel_buffer.h"
#include "M8_devignette.h"
#include "dng_string_win.h"
#include "dng_xmp.h"
#include "dng_macbeth_info.h"

#if qWinOS
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
#else
#include <Cocoa/Cocoa.h>
#endif


enum
{

// constants.	
kcpfStateNone = 0,
kcpfStateCreated,
kcpfStateSaved

};


enum
{
	CornerFix_bad_Bayer = dng_error_unknown+100,	//< CornerFix doesn't understand the Bayer pattern.
	CornerFix_bad_linear,							//< CornerFix doesn't understand the linear raw format.	
	CornerFix_bad_pixel_format,						//< CornerFix doesn't understand the pixel format.	
	CornerFix_unknown_model,						//< Not a supported model.
	CornerFix_saturated								//< Image contains saturated pixels.
};



class dng_win_glue
{
public:
	dng_win_glue(void);

#if qWinOS
	void setImage(String ^file);
	void setOutputFile(String ^file);
	bool setCPFFile(String ^file);
	void setCSVFile(String ^file);
	void setTIFFile(String ^file);
	bool saveCPFFile(String ^file);
#else
	void setImage(NSString *file);
	void setOutputFile(NSString *file);
	bool setCPFFile(NSString *file);
	void setCSVFile(NSString *file);
	void setTIFFile(NSString *file);
	bool saveCPFFile(NSString *file);
#endif
	void setBuildCPF();
	void setVerbose(bool);
	void setMultiple(bool);
	void setCompress(bool);
	void setDisableIlluminant(bool);
	int getCPFState();
	void resetCPF();
	dng_error_code dng_validate ();
	void SetAntiAlias(int strength);
	void SetChromaStrength(int strength);
	void SetLumaStrength(int strength);
	void setLossless(bool val);
	void setExifUpdate(bool val);
	void setCalculateAperture(bool val);
	void setClipApertures(bool val);
	void setMaximizeResolution(bool val);
	void setOverwriteCameraName(bool val);
	void setAsymetry(real64 strength);

private:
	void zeroOutInfoBlacks (dng_linearization_info *info);

private:
	
uint32 gMathDataType;

bool gFourColorBayer;
		
int32 gMosaicPlane;

uint32 gMinStage3Size;

const dng_color_space *gFinalSpace;

uint32 gFinalPixelType;

dng_string_win gImageDNG;
dng_string_win gDumpStage1;
dng_string_win gDumpStage2;
dng_string_win gDumpStage3;
dng_string_win gDumpTIF;
dng_string_win gDumpDNG;
dng_string_win gDumpCSV;
dng_string_win gDumpCPF;
dng_string_win gDumpC;

bool doIlluminant;
int  cpfState;
bool buildCPF;
bool multiple;
bool buildHires;
bool exifUpdate;
bool calculateApertures;
bool clipApertures;
bool maximizeResolution;
bool overwriteCameraName;
uint32 compressTag;
int chromaStrength;
int lumaStrength;

M8_devignette * deVignetteInfo;

AutoPtr<dng_image> newStage1Image;

};


