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
* ThumbNSImage.h
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
*
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

#import <Cocoa/Cocoa.h>


@interface ThumbNSImage : NSImage {
	

}

- (ThumbNSImage*) thumbNailInitFromFilename:(NSString *) filename minSize: (NSPoint) minSize;
- (ThumbNSImage*) preferredInitFromFilename:(NSString *) filename minSize: (NSPoint) minSize thumbPreferred: (BOOL) thumbPreferred;

@end
