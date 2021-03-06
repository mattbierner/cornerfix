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
 * thumbnailWrapper.mm
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 29 Nov 2010 : 1.4.0.4
 *
 */

/*
 * The wrapper for the tiffThumb code.
 */

#import "thumbnailWrapper.h"
#ifdef __cplusplus
#include "tiffThumb.h"
#endif


@implementation thumbnailWrapper

+ (NSImage*) thumbnailFromFilename:(NSString *) filePath
{
	NSImage *retVal = NULL;
	size_t thumbSize = 0;
	uint8_t *thumbData = nil;
	
	tiffThumb *decoder = new tiffThumb();
	
	if (decoder->parseFile([filePath UTF8String])) {
		decoder->getBuffer(&thumbSize, &thumbData);
	}
	else {
		retVal = NULL;
	}
	
	NSData *imageData = [NSData dataWithBytes:thumbData length:thumbSize];
	
	retVal = [[[NSImage alloc] initWithData:imageData] autorelease];
	
	delete decoder;
	return retVal;
}

@end
