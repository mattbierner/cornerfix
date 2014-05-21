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
* CornerFixController
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
* 21 Jun 2007 : 0.9.0.6
* 1 Jul 2007 : 0.9.1.0
* 1 Sept 2007 : 0.9.1.0 Mac version
* 20 Dec 2007 : 0.9.1.8
* 19 Sept 2009 : 1.3.0.0
*
*/

/*
* The main app form for CornerFix for the Mac.
*/


#import <Cocoa/Cocoa.h>
#include "CFViewController.h"

@interface CornerFixController : NSObject
{
    IBOutlet NSImageView *correctedImage;
    IBOutlet NSImageView *originalImage;
    IBOutlet NSTextField *statusField;
	IBOutlet NSWindow *window;
	IBOutlet NSMenu *mainMenu;
	IBOutlet CFViewController *viewController;
	IBOutlet NSSplitView *splitView;
	IBOutlet NSWindow *progressSheet;
    IBOutlet NSProgressIndicator *sheetProgress;
    IBOutlet NSTextField *sheetStatusField;
	
	NSString * tempFile;
	NSArray * loadedCPFFiles;
	NSArray  * imageFiles;
	NSArray * cpfFiles;
	NSArray * outputFiles;
	NSString * readyMsg;
	NSString * imageDirectory;
	NSString * cpfDirectory;
	int progressState;
	int validateRetVal;
	bool batchMode;	
	void * glue;
	NSFileManager * fileManager;
	bool disableMenu;
	long long prefsSignature;
	bool sheetNotDone;

}

- (void) runWorkerAsync:(id)param;
- (void) RunWorkerCompleted:(id)param;
- (void) processImages;
- (void) updateDisplay;

- (void) windowWillClose:(NSNotification *)notification;

- (BOOL) prefsSignatureChanged;

  // File handling
- (IBAction) openDocument: sender;
- (IBAction) saveDocument: sender;
- (IBAction) batchDocument: sender;
- (IBAction) openPreferencesWindow:(id)sender;
- (IBAction) openProfile: sender;
- (IBAction) createProfile: sender;
- (IBAction) saveProfile: sender;
- (IBAction) batchProfile: sender;
- (IBAction) imageApplyHandler: sender;
- (void)prefWindowClose:(NSNotification *)notification;
- (void)updateProgress:(NSNumber *) myNumber;

	//App delegate stuff
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
@end
