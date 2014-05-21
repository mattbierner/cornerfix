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
* ThumbNSImage.m
* ---------------
* (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
*
* Original Author:  Sandy McGuffog;
* Contributor(s):   -;
*
*
* Changes
* -------

* 11 Sept 2007 : 0.9.1.3
* 5 Nov 2007 : 0.9.1.6
* 27 Dec 2008 : 1.0.0.0 : Fix memory leak
* 27 Dec 2008 : 1.0.0.0 : Fix memory leak
* 16 Oct 2010 : 1.4.0.1 : Fix crash on non-existent file....
* 4 July 2011 : 1.4.2.0 : Fix memory bug on Lion
* 4 July 2011 : 1.4.2.4 : Fix overrelease under error conditions
 */

/*
 * By default, NSImage entirely rerenders any image that its given. That is slow, and a 
 * problem if the native file format isn't recognised. This extension allows thumbnails to be
 * loaded by preference for speed. Note: If a thumbnail doesn't exist, the image is rerendered, and if
 * the image can't be rendered, and a thumnail is available, it will be shown 
 * In addition, a minimum thumbnail size can be specified - if the thumbnail is smaller than this,
 * it will be integer scaled. This allows NSImageViews to be used in proportional scaling mode
 * without running into the "doesn't scale beyond original image size" issue
 */

#import "ThumbNSImage.h"



@implementation ThumbNSImage

- (ThumbNSImage*) thumbNailInitFromFilename:(NSString *) filename minSize: (NSPoint) minSize
{
	
	CFStringRef path = CFStringCreateWithCString(NULL, [filename UTF8String], kCFStringEncodingUTF8);
	CFURLRef url = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
	CFRelease(path);
	CGImageSourceRef imagesource = CGImageSourceCreateWithURL(url, NULL);
	CFRelease(url);
	
	if (imagesource) {

		NSDictionary* thumbOpts = [[NSDictionary alloc] initWithObjectsAndKeys:
								   (id)kCFBooleanTrue, (id)kCGImageSourceCreateThumbnailWithTransform,
								   (id)kCFBooleanTrue, (id)kCGImageSourceCreateThumbnailFromImageIfAbsent,
								   nil];
		
		CGImageRef cgImage = CGImageSourceCreateThumbnailAtIndex(imagesource, 0, (CFDictionaryRef)thumbOpts);
		CFRelease(imagesource);
		[thumbOpts release];
		
		if (cgImage) {
			NSRect imageRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
			CGContextRef imageContext = nil;
			
			// Get the image dimensions.
			imageRect.size.height = CGImageGetHeight(cgImage);
			imageRect.size.width = CGImageGetWidth(cgImage);
			
			float scale =  ceil(minSize.y / imageRect.size.height);
			if (ceil(minSize.x / imageRect.size.width) > scale) scale = ceil(minSize.x / imageRect.size.width);
			
			if (scale > 8.0) scale = 8.0;
			
			if (scale > 1.0) {
				imageRect.size.height = imageRect.size.height * scale;
				imageRect.size.width = imageRect.size.width * scale;			
			}
			
			// Create a new image to receive the Quartz image data.
			[self initWithSize:imageRect.size];
			[self lockFocus];
			
			// Get the Quartz context and draw.
			imageContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			CGContextDrawImage(imageContext, *(CGRect*)&imageRect, cgImage);
			[self unlockFocus];
			CGImageRelease(cgImage);
			return self;
		}
		else {
			return nil;
		}
	}
	else {
		return nil;
	}	
}

- (ThumbNSImage*) preferredInitFromFilename:(NSString *) filename minSize: (NSPoint) minSize thumbPreferred: (BOOL) thumbPreferred
{
	if (thumbPreferred) {
		if ([self thumbNailInitFromFilename:filename minSize: minSize]) {
			return self;
		}
		else {
            // NOTE: here self has already been autoreleased.......
			return [[ThumbNSImage alloc] initWithContentsOfFile:filename];
		}
	}
	else {
		if ([self initWithContentsOfFile:filename]) {
			return self;
		}
		else {
            // NOTE: here self has already been autoreleased.......
			return [[ThumbNSImage alloc] thumbNailInitFromFilename:filename minSize: minSize];
		}
	}
	
}

@end
