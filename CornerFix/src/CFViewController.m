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
* CFViewController.m
* ---------------
* (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
*
* Original Author:  Sandy McGuffog;
* Contributor(s):   -;
*
*
* Changes
* -------
* 7 Sept 2007 : 0.9.1.3
*
*/

/*
 * This sorts out some of teh more egregiously bad behavior of the NSSplitterView class
 * It functions as a delegate. In most languages, we would subclass, but this objective-C(!)
 */

#import "CFViewController.h"


@implementation CFViewController

- (id) init
{
	self = [super init];
	if (self != nil) {
		splitFactor = 0.5;
	}
	return self;
}

- (void)splitView:(NSSplitView *)sender resizeSubviewsWithOldSize:(NSSize)oldSize
{
	NSArray *subviewList = [sender subviews];
	NSView *leftView = [subviewList objectAtIndex:0];
	if ([leftView frame].origin.x != 0) {
		leftView = [subviewList objectAtIndex:1];
	}
	
//	NSLog([NSString stringWithFormat:@"resizeSubviewsWithOldSize %f, %f, splitFactor %f", oldSize.width, oldSize.height, splitFactor]);
	NSRect leftFrame;
	leftFrame.size.width = (oldSize.width - [sender dividerThickness])*splitFactor; 
	leftFrame.size.height = oldSize.height;
	leftFrame.origin = NSMakePoint(0,0);
	
	[leftView setFrame:leftFrame];
	
	[sender adjustSubviews];

}



- (CGFloat)splitView:(NSSplitView *)sender constrainSplitPosition:(CGFloat)proposedPosition ofSubviewAt:(NSInteger) __unused offset
{
	splitFactor = (proposedPosition+[sender dividerThickness]/2)/((float) [sender frame].size.width);
//	NSLog([NSString stringWithFormat:@"constrainSplitPosition %f, %f, splitFactor %f", proposedPosition, [sender dividerThickness], splitFactor]);

	return proposedPosition;
}
	


- (CGFloat)splitView:(NSSplitView *) __unused sender constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger) offset
{
//	NSLog([NSString stringWithFormat:@"constrainMinCoordinate %i:%f", offset, lhsSide]);
	if (offset == 0) {
		return 30;
	}	
	else {
		return proposedMin;
	}
}

- (CGFloat)splitView:(NSSplitView *)sender constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)offset
{
//	NSLog([NSString stringWithFormat:@"constrainMaxCoordinate %i:%f", offset, rhsSide]);
	if (offset == 0) {
		return ((float) [sender frame].size.width - (30 + [sender dividerThickness]));
	}
	else {
		return proposedMax;
	}
		
}



- (void) dealloc
{
	
	[super dealloc];
}

@end
