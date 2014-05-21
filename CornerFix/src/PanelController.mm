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
* PanelController.m
* ---------------
* (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
*
* Original Author:  Sandy McGuffog;
* Contributor(s):   -;
*
*
* Changes
* -------
* 1 Sept 2007 : 0.9.1.0 Mac version
*
*/

/*
* The text panel for CornerFix for the Mac.
*/

#import "PanelController.h"
#import "dng_local_printf.h"
#include "CornerFixVersion.h"


@implementation PanelController

- (id)init
{
	self = [super initWithWindowNibName:@"TextPanel"];
	return self;
}

- (void)windowDidLoad
{
#ifdef __debug
	NSLog(@"TextPanel.nib loaded");
#endif
}

- (void)addTextToPanel:(NSString *) myText 
{
	NSRange endRange;
 
    endRange.location = [[text textStorage] length];
    endRange.length = 0;
    [text replaceCharactersInRange:endRange withString:myText];	
    endRange.location = [[text textStorage] length]-1;
    endRange.length = 1;
    [text scrollRangeToVisible:endRange];	
}

@end

@class PanelController;

@implementation TextController

- (IBAction)showTextPanel:(id) __unused sender;
{
	// Is panelContrller nil?
	if (!panelController) {
		panelController = [[PanelController alloc] init];		
		[panelController showWindow:self];
		dng_local_printf::setBox(panelController);
		
		dng_local_printf::printf(CornerFixNameString " Version " CornerFixVersion "\nCopyright © 2007-2011 Sandy McGuffog\n"
								"Portions Copyright © 2006 Adobe Systems Incorporated\nAll Rights Reserved.\n"
								"Lossless JPEG code adapted from:\n"
								"Copyright (C) 1991, 1992, Thomas G. Lane.\n" 
								"Part of the Independent JPEG Group's software.\n" 
								"See the file Copyright for more details.\n" 
								"Copyright (c) 1993 Brian C. Smith,\n" 
								"The Regents of the University of California\n" 
								"All rights reserved.\n"
		
							   "Copyright (c) 1994 Kongji Huang and Brian C. Smith.\n" 
							   "Cornell University\n" 
							   "All rights reserved.\n" 
							   "Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without written agreement is hereby granted, provided that the above copyright notice and the following two paragraphs appear in all copies of this software.\n" 
							   "IN NO EVENT SHALL CORNELL UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF CORNELL UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. \n" 
														"CORNELL UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN ""AS IS"" BASIS, AND CORNELL UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\n"
								"DBPrefsWindowController code adapted from Dave Batton http://www.Mere-Mortal-Software.com/blog/ Copyright 2007. Some rights reserved.\n"
								"CornerFix Icon adapted from Carlitus (Carles Carbonell Bernado)'s KDE icon set\n\n"
								
							   "This program is free software; you can redistribute it and/or\n"
							   "modify it under the terms of the GNU General Public License\n"
							   "as published by the Free Software Foundation; either version 2\n"
							   "of the License, or (at your option) any later version.\n"
							   "\n"
							   "This program is distributed in the hope that it will be useful,\n"
							   "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
							   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
							   "GNU General Public License for more details.\n"
							   "\n"
							   "You should have received a copy of the GNU General Public License\n"
							   "along with this program; if not, write to the Free Software\n"
								"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n");	
									   
		}
else {
			if ([[panelController window] isVisible]) {
			[[panelController window] orderOut: nil];
		}
		else {
			[panelController showWindow:self];
		}
	}
		
}

- (void) dealloc
{
	[panelController release];
	[super dealloc];
}
@end
