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
 * tiffThumb.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 15 June 2008 : Initial version for CornerFix 1.1.0.0
 * 7 Mar 2010   : CornerFix 1.3.0.6; fix to allow JPEG format thumbnails
 *
 */

/*
 * This code extracts the thumbnail from a DNG file and turns it into a standalone 
 * TIFF or JPEG format file.
 * This was prompted by Windows 7, which (sigh) is no longer capable of displaying the 
 * thumbnail of a DNG file in a .Net PictureBox control, which all previous version of 
 * Windows did. This in turn broke the previous version of CornerFix
 *
 * Although written for CornerFix, tiffThumb is completely standalone, as it contains all
 * the code in it to decode enough of a DNG file to find the parts it needs. AKA, 
 * tiffThumb does not use or depend on the DNG SDK. This was done because (a) the DNG
 * SDK doesn't actually extract thumbnails, and to make it do so is quite difficult and 
 * (b) this code is useful standalone.
 * Note that while that means tiffThumb understands subsets of DNG and tiff formats
 * (actually the same thing), this code does not actually understand the image data - 
 * it just copies image data verbatim.
 * If the thumbnail is in JPEG format, then then tiffThumb actually produces a JPEG file.
 * In that case, in order to get image orientation right, it will also insert an APP1 
 * EXIF section into the file, as a pure JFIF JPEG doesn't have any way to specify
 * orientation. 
 * Documentation for the API is in tiffThumb.h
 */

#ifndef _MSC_VER
// So, of course, MS doesn't support stdint.h(!)
#include <stdint.h>
#else
// So we have to define our own MS specific stuff
typedef __int8            int8_t;
typedef __int16           int16_t;
typedef __int32           int32_t;
typedef __int64           int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
// Also, there are a bunch of security warnings that try to get you 
// to use MS only functions
#ifndef _CRT_SECURE_NO_DEPRECATE 
#define _CRT_SECURE_NO_DEPRECATE 1
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stddef.h>

#include <errno.h>

//#ifndef __tiffThumbIncluded
#define __tiffThumbIncluded 1


enum TIFFStringLength {
	kTIFFMaxStringLength = 72,
};


// Directory Entry
struct TIFFDirectory {
	uint8_t	tag[2];									// Which tag is this?
	uint8_t	tagType[2];								// What kind of data does it hold?
	uint8_t	count[4];								// Number of values
	uint8_t	valueOffset[4];							// either the value or the offset, depending on tagType
};

// Magic numbers.

enum TIFFMagicNumbers
{
	magicTIFFLocal					= 42,			// TIFF and DNG
	magicExtendedProfileLocal		= 0x4352,
	magicPanasonicLocal				= 85,
	magicOlympusALocal				= 0x4F52,
	magicOlympusBLocal				= 0x5352
};


class tiffThumb
	{
	public:
		//////////////////////////////////////////////////////////////
		//
		// Class initialiser
		//
		//////////////////////////////////////////////////////////////		
		tiffThumb ();
		
		virtual ~tiffThumb ();
		
		//////////////////////////////////////////////////////////////
		//
		// File parser
		//
		//////////////////////////////////////////////////////////////
		// in_file : Zero terminated DNG file location
		// imageNumber : image select for multi-image files
		//
		// returns true if image data at any resolution could be read (see getErrorString)
		//
		// imageNumber defaults to zero. Note that a DNG file should actually only
		// have a single image; the Image number setting is for use with TIFF images.
		//
		// Note that tiffThumb only extracts thumnails that are in strip format
		// (which is a requirement, so shouldn't be a significant restriction.
		// Also note that tiffThumb requires a "sub IFD" structure. Again, this is 
		// required so shouldn't be a problem
		virtual bool parseFile (const char *in_file);
		virtual bool parseFile (const char *in_file, int ImageNumber);		
		
		//////////////////////////////////////////////////////////////
		//
		// get Width
		//
		//////////////////////////////////////////////////////////////
		// Returns the actual image width
		virtual size_t getWidth();
		
		//////////////////////////////////////////////////////////////
		//
		// get Height
		//
		//////////////////////////////////////////////////////////////
		// Returns the actual image height
		virtual size_t getHeight();
		
		//////////////////////////////////////////////////////////////
		//
		// Get Error String
		//
		//////////////////////////////////////////////////////////////
		// Returns a zero terminated error string which contains an English language 
		// error description, or an empty string (0x0 as the first character), if no error
		// information is available. The error string is set by the parseFile function.
		// If parseFile return false, then the contents of the error string consitute 
		// an error message, and no image data is available. If parseFile returns true
		// then the contents of the error string consitute a warning.
		virtual char *getErrorString();
		
		//////////////////////////////////////////////////////////////
		//
		// get Buffer
		//
		//////////////////////////////////////////////////////////////
		// Returns a pointer to a memory buffer with the TIFF format
		// file image in it
		// NOTE: This is a malloc'd buffer - but controlled by tiffThumb
		// it will be released when the tiffTumb instance is deleted
		virtual void getBuffer(size_t *size, uint8_t **buffer);
		
		//////////////////////////////////////////////////////////////
		//
		// setMagicNumberCheck
		//
		//////////////////////////////////////////////////////////////
		// If this set to a non-zero value, the parser will check that 
		// the files's magic number is as specified.
		// A DNG/TIFF file's magic number is 42
		// The default is zero: aka no magic number check is done.
		virtual void setMagicNumberCheck(uint32_t magicNumber);
		
		//////////////////////////////////////////////////////////////
		//
		// setVerbose
		//
		//////////////////////////////////////////////////////////////
		// If set, useful file structure information is printed out
		// Note this is to console, so this is only usable in 
		// command line environments.
		virtual void setVerbose(bool val);

		//////////////////////////////////////////////////////////////
		//
		// get JPEG status
		//
		//////////////////////////////////////////////////////////////
		// Returns true is this is a JPEG thumbnail; you must call this 
		// to find out in what format the returned file image is.
		virtual bool getIsJPEG();		
		
	protected:
		bool isLittleEndian;
		bool isJPEG;
		char errorString[kTIFFMaxStringLength*3];
		FILE *fp;
		
		uint16_t getTIFF16(uint8_t buffer[]);
		
		uint32_t getTIFF32(uint8_t buffer[]);
		void writeTIFF32(uint8_t buffer[], uint32_t val);
		void writeTIFF16(uint8_t buffer[], uint32_t val);
		void writeJPEG16(uint8_t buffer[], uint32_t val);
		
		struct TIFFDirectory *stripByteCounts;
		
		uint8_t *outputFileBuffer;
		size_t totalMemory;
		uint32_t magicCheck;
		bool dumpTags;
		size_t width, length;
		
		
		void dumpTagDescription(TIFFDirectory *tag);
		int tagPass(TIFFDirectory *tag);
		size_t parseTag(TIFFDirectory *tag);
		uint8_t *getData(uint8_t *mem, uint32_t dataOffset, size_t size);
		uint8_t *getTagData(TIFFDirectory *tag);
		void copyTag(TIFFDirectory *tag, uint8_t *dest, int *tagNum, uint32_t *metaOffset, uint32_t *imageOffset, int skipBytes);
		uint8_t getTagByte(void *buffer, uint32_t index);
		char getTagASCII(void *buffer, uint32_t index);
		uint16_t getTagShort(void *buffer, uint32_t index);
		uint32_t getTagLong(void *buffer, uint32_t index);
		uint32_t getTagUIntValue(void *buffer, uint16_t tagType, uint32_t index);
		void writeTagData(uint8_t *theData, size_t size, uint8_t *dest, int *tagNum, uint32_t *metaOffset);

	};

//#endif
