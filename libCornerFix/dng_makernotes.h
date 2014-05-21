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
 * dng_makernotes.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 24 October 2007 : Original version;
 * 29 November 2007 : 0.9.1.7;
 * 2 December 2008 : 1.0.0.0;
 * 12 June 2009 : 1.1.0.0;
 * 21 Nov 2009 : 1.3.0.4
 * 23 January 2011 : 1.4.1.0
 * 4 July 2011 : 1.4.2.0
 *
 */

/*
 * Decodes and stores maker specific data
 */

#pragma once

#include "dng_shared.h"

#define MAX_LEICA_LENSES 54
#define MAX_LEICA_FRAMELINES 4

enum
	{
	tcJPEGFileQuality = 0x300, //768
	tcSelectedUserProfile = 0x302, //770
	tcMakerNoteCameraSerialNumber = 0x303, //771
	
	tcWhiteBalancePreset = 0x304, //772
	
	tcLensId = 0x310, //784
		
	tcBlueDotBrightness = 0x311, //785
	tcTTLBrightness = 0x312, //786
	
	tcEstimatedAperture = 0x313, //787
	
	tcSystemTemperature = 0x320, //800
	tcColorTemperature = 0x321, //801
	
	tcUVIRCorrection = 0x325, //805
	tcRedAsShotNeutral = 0x322, //802
	tcGreenAsShotNeutral = 0x323, //803
	tcBlueAsShotNeutral = 0x324, //804
	tcCCDID = 0x330, //816
	tcCCDBoardID = 0x331, //817
	tcControllerBoardID = 0x332, //818
	tcM16CID = 0x333, //819
	tcUniqueImageId = 0x340, //832
		
	tcM9LensId = 0x3405, // 13317
	tcM9EstimatedAperture = 0x3406, // 13318
	tcM9TTLBrightness = 0x3407, // 13319	
	tcM9BlueDotBrightness = 0x3408, // 13320
	
};



class dng_makernotes
{
	public:
	
		uint32 fParent;	
		uint32 fJPEGFileQuality;
		uint32 fSelectedUserProfile;
		uint32 fMakerNoteCameraSerialNumber;
		
		uint32 fWhiteBalancePreset;
		
		uint32 fLensId;
		
		uint32 fFrameLineSelector;
		
	
		dng_srational fBlueDotBrightness;
		dng_srational fTTLBrightness;
		
		dng_srational fEstimatedAperture;
	
		dng_srational fCalculatedAperture;
		
		uint32 fSystemTemperature;
		uint32 fColorTemperature;
		
		uint32 fUVIRCorrection;
		dng_srational fRedAsShotNeutral;
		dng_srational fGreenAsShotNeutral;
		dng_srational fBlueAsShotNeutral;
		uint32 fCCDID;
		uint32 fCCDBoardID;
		uint32 fControllerBoardID;
		uint32 fM16CID;
		uint32 fUniqueImageId;
		int32 fM9BlueDotBrightness;
		int32 fM9TTLBrightness;

	public:
	
		dng_makernotes ();
		
		virtual ~dng_makernotes ();
		
		virtual bool ParseTag (dng_stream &stream,
							   uint32 parentCode,
							   uint32 tagCode,
							   uint32 tagType,
							   uint32 tagCount,
							   uint64 tagOffset);
							   
		virtual void PostParse ();
		
		virtual bool IsValidDNG (dng_shared &shared,
								 uint32 parentCode);

		virtual void DumpMakerNotes();

		virtual const char * GetLensName();

		virtual void setClipApertures(bool val);

	private:
		char buf[BUF_LENGTH];
		bool clipApertures;


};
