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
* CornerFixController.mm
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
* 4 Sept 2007 : 0.9.1.1
* 5 Sept 2007 : 0.9.1.2 - DMR hack
* 11 Sept 2007 : 0.9.1.3
* 25 Oct 2007 : 0.9.1.6
* 29 Nov 2007 : 0.9.1.7
* 20 Dec 2007 : 0.9.1.8
* 20 Dec 2007 : 1.0.0.0
* 12 Sept 2009 : 1.2.0.0
* 18 Oct 2009 : 1.3.0.0
* 6 Oct 2010 : 1.4.0.0
* 15 Oct 2010 : 1.4.0.1
* 21 Oct 2010 : 1.4.0.2
* 29 Nov 2010 : 1.4.0.4
* 4 July 2011 : 1.4.2.0
* 31 July 2012 : 1.4.2.2
* 18 Sept 2013 : 1.4.2.3
* 5 Oct 2013 : 1.4.2.4
*
*/

/*
* The main app form for CornerFix for the Mac.
*/

#import "CornerFixController.h"

#import "PrefsWindowController.h"
#include "CornerFixVersion.h"
#include "ThumbNSImage.h"
#import "thumbnailWrapper.h"
#include "CornerFixFile.h"

#include "dng_local_printf.h"
#include "dng_win_glue.h"
#include "dng_errors.h"

#include <unistd.h>


@implementation CornerFixController

- (id)init
{
	self = [super init];
	glue = new dng_win_glue();
	tempFile = nil;
	loadedCPFFiles = nil;
	readyMsg = @"Ready......";
	progressState = -1;
	validateRetVal = 0;
	batchMode = false;
	disableMenu = false;
	imageDirectory = nil;
	cpfDirectory = nil;
	sheetNotDone = YES;

	fileManager = [NSFileManager defaultManager];
	
	//Now we jump through some major hoops to get a temporary file name................
	NSString *sourcePath = NSTemporaryDirectory();	
	if (sourcePath) {
		sourcePath =[sourcePath stringByAppendingPathComponent: @"CornerFixXXXXXX.tmp"];	
		char tmpfile[500];
		strcpy(tmpfile, [sourcePath UTF8String]);

		int fd = mkstemps(tmpfile,4); // check return for -1, which indicates an error
		
		if (fd >= 0) {
			tempFile = [[NSString stringWithCString: tmpfile encoding:NSUTF8StringEncoding] retain];
		
			NSFileHandle *myhandle = [[NSFileHandle alloc] initWithFileDescriptor:fd];
			[myhandle closeFile];
			glue->setOutputFile(tempFile);
			[myhandle release];
		}
	}
	
	if (!tempFile) {
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		//		[alert addButtonWithTitle:@"Cancel"];
		[alert setMessageText:@"Could not create a temporary file - Exiting"];
		[alert setInformativeText:@"Could be a permissions problem - try running Disk Utility and repairing disk permissions"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		
		if ([alert runModal] == NSAlertFirstButtonReturn) {
			// OK clicked
		}
		[alert release];
		[NSApp terminate:nil];		

	}	
	
	// Now we need to register the various defaults for the preferences 
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSMutableDictionary *appDefaults = [NSMutableDictionary dictionary];
	
	[appDefaults setObject:@"NO" forKey:@"verboseMessages"];
	[appDefaults setObject:@"NO" forKey:@"multipleEquations"];
	[appDefaults setObject:@"YES" forKey:@"luminanceCompensation"];
	[appDefaults setObject:[NSNumber numberWithInt:0] forKey:@"antiAliasStrength"];
	[appDefaults setObject:[NSNumber numberWithInt:100] forKey:@"luminance"];
	[appDefaults setObject:[NSNumber numberWithInt:100] forKey:@"chroma"];
	[appDefaults setObject:@"NO" forKey:@"levelCompression"];
	[appDefaults setObject:@"NO" forKey:@"losslessCompression"];
	[appDefaults setObject:@"" forKey:@"imageDirectory"];
	[appDefaults setObject:@"" forKey:@"cpfDirectory"];
	[appDefaults setObject:@"YES" forKey:@"preferThumbNails"];
	[appDefaults setObject:@"NO" forKey:@"updateEXIF"];
	[appDefaults setObject:@"YES" forKey:@"calculateAperture"];
	[appDefaults setObject:@"YES" forKey:@"clipAperture"];
	[appDefaults setObject:@"NO" forKey:@"maximizeResolution"];
	[appDefaults setObject:@"YES" forKey:@"bidirectionalCorrection"];
	
#ifdef DMR_HACK
	[appDefaults setObject:@"YES" forKey:@"overwriteCameraName"];
#endif
	[userDefaults registerDefaults:appDefaults];
	
	[userDefaults addObserver:self
				  forKeyPath:@"antiAliasStrength"
					 options:(NSKeyValueObservingOptionNew |
							  NSKeyValueObservingOptionOld)
					 context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"luminance"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"chroma"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"levelCompression"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"losslessCompression"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"preferThumbNails"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"updateEXIF"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"calculateAperture"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"clipAperture"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	[userDefaults addObserver:self
				   forKeyPath:@"maximizeResolution"
					  options:(NSKeyValueObservingOptionNew |
							   NSKeyValueObservingOptionOld)
					  context:NULL];
	
	[self prefsSignatureChanged];
	
	//Now get the directory defaults....
	imageDirectory = [[[NSUserDefaults standardUserDefaults] stringForKey:@"imageDirectory"] retain];
	if ((imageDirectory) && ([imageDirectory isEqualToString:@""])) {
		[imageDirectory release];
		imageDirectory = nil;
	}
	cpfDirectory = [[[NSUserDefaults standardUserDefaults] stringForKey:@"cpfDirectory"] retain];
	if ((cpfDirectory) && ([cpfDirectory isEqualToString:@""])) {
		[cpfDirectory release];
		cpfDirectory = nil;
	}
	
	dng_local_printf::setWorker(self);
	
	
	return self;
}

- (void) wakeupProgressSheet {
	if (!progressSheet) {
		//Check the ProgressSheet instance variable to make sure the custom sheet does not already exist.
		[NSBundle loadNibNamed: @"ProgressSheet" owner: self];
		[progressSheet setOpaque:NO]; // YES by default
		NSColor *semiTransparentBlue =
			[NSColor colorWithDeviceRed:0.0 green:0.0 blue:0.3 alpha:0.7];
		[progressSheet setBackgroundColor:semiTransparentBlue];
		
		[progressSheet setAlphaValue:0.9];
	}
}


- (void) awakeFromNib {	
	// this is required because Interface Builder doesn't work right.......
	[splitView setDelegate:viewController];
	
//	[sheetProgress setDisplayedWhenStopped:NO];
	[self updateDisplay];
	[self wakeupProgressSheet];
}

//App delegate stuff
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *) __unused application
{
	return YES;	
}

- (void)windowWillClose:(NSNotification *) __unused notification
{

    NSUserDefaults *userDefaults;
	userDefaults = [NSUserDefaults standardUserDefaults];
    
	if (imageDirectory) {
		[userDefaults setObject:imageDirectory forKey:@"imageDirectory"];
	}
	if (cpfDirectory) {
		[userDefaults setObject:cpfDirectory forKey:@"cpfDirectory"];
	}
	[userDefaults synchronize];

    if (tempFile) {
		[fileManager removeItemAtPath:tempFile error:NULL];;
		tempFile = nil;
	}
    
	if (glue) {
		delete glue;
		glue = nil;
	}

}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)__unused app
{
    if (tempFile) {
		[fileManager removeItemAtPath:tempFile error:NULL];;
		tempFile = nil;
	}
    
	if (glue) {
		delete glue;
		glue = nil;
	}
	
	return NSTerminateNow;
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{	
   SEL theAction = [anItem action];
	
	//	if (theAction == @selector(terminate:))
		{
			return YES;
		}
		//else {
		//	return !disableMenu;
		//}
}

- (BOOL) prefsSignatureChanged
{
	long long newSignature;	
	newSignature =	(([[NSUserDefaults standardUserDefaults] integerForKey:@"antiAliasStrength"]) << 30) |
					(([[NSUserDefaults standardUserDefaults] integerForKey:@"luminance"]) << 18) |
					(([[NSUserDefaults standardUserDefaults] integerForKey:@"chroma"]) << 6) |
					(([[NSUserDefaults standardUserDefaults] boolForKey:@"levelCompression"]) ? 0 : 32) |
					(([[NSUserDefaults standardUserDefaults] boolForKey:@"losslessCompression"]) ? 0 : 16) |
					(([[NSUserDefaults standardUserDefaults] boolForKey:@"updateEXIF"]) ? 0 : 8) |
					(([[NSUserDefaults standardUserDefaults] boolForKey:@"calculateAperture"]) ? 0 : 4) |
					(([[NSUserDefaults standardUserDefaults] boolForKey:@"clipAperture"]) ? 0 : 2) |
					(([[NSUserDefaults standardUserDefaults] boolForKey:@"maximizeResolution"]) ? 0 : 1) 
					;
	if (newSignature != prefsSignature) {		
		prefsSignature = newSignature;
		return true;
	}
	else {
		return false;
	}
}


// error alert sheet
- (void)alertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	sheetNotDone = NO;
}


- (void)showProgressSheet
{
	[self wakeupProgressSheet];	
	
    [NSApp beginSheet: progressSheet
	   modalForWindow: window
		modalDelegate: self
	   didEndSelector: @selector(didEndSheet:returnCode:contextInfo:)
		  contextInfo: nil];
}


- (void)didEndSheet:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut:self];
}


- (void) updateDisplay
{
	if (imageFiles && ([imageFiles count] > 0 ))
	{
		NSImage *image = nil;
		[window setTitle:[[imageFiles objectAtIndex:0] lastPathComponent]];

		image = [[[ThumbNSImage alloc]
			preferredInitFromFilename:[imageFiles objectAtIndex:0] 
						minSize: NSMakePoint(1024,  768)
						thumbPreferred: [[NSUserDefaults standardUserDefaults] boolForKey:@"preferThumbNails"]] autorelease];
		if (!image || (NSEqualSizes([image size], NSZeroSize) == YES)) {
			image = [thumbnailWrapper thumbnailFromFilename:[imageFiles objectAtIndex:0]];
		}
		
		if (!image || ![image isValid])
		{	
			NSAlert *alert = [[NSAlert alloc] init];
			[alert addButtonWithTitle:@"OK"];
			//		[alert addButtonWithTitle:@"Cancel"];
			[alert setMessageText:@"Mac OS cannot display that file"];
			[alert setInformativeText:@"Check that you have the latest version of Mac OS installed"];
			[alert setAlertStyle:NSWarningAlertStyle];
			
			if ([alert runModal] == NSAlertFirstButtonReturn) {
				// OK clicked
			}
			[alert release];				
			
		};
		[originalImage setImage:image];
	}
	else {
		[window setTitle:@CornerFixNameString];
	}
	
	switch (glue->getCPFState()) {
		case kcpfStateNone:
			readyMsg = @"Ready....";
			break;
		case kcpfStateCreated:
			readyMsg = [@"Profile: <" stringByAppendingString:[[[cpfFiles objectAtIndex:0]lastPathComponent] stringByAppendingString:@">...."]];
			[window setTitle:[[window title] stringByAppendingString:[@" <" stringByAppendingString:[[[cpfFiles objectAtIndex:0]lastPathComponent] stringByAppendingString:@">"]]]];
			break;
		case kcpfStateSaved:
			readyMsg = [@"Profile: " stringByAppendingString:[[[cpfFiles objectAtIndex:0]lastPathComponent] stringByAppendingString:@"...."]];
			[window setTitle:[[window title] stringByAppendingString:[@" <" stringByAppendingString:[[[cpfFiles objectAtIndex:0]lastPathComponent] stringByAppendingString:@">"]]]];
			break;
		default:
			readyMsg = @"Ready....";
			break;
	}
    
    if (progressSheet)
        [sheetStatusField setStringValue:readyMsg];
}


- (void) backgroundWorker1_DoWork {
	validateRetVal = dng_error_none;
    int index = 0;
    for (NSString* file in imageFiles)
    {
        if (validateRetVal == dng_error_none)
        {
            if (([fileManager fileExistsAtPath:file]) && ([fileManager isReadableFileAtPath:file]))
            {
                if ((outputFiles) && ([outputFiles count] > 0))
                {
                    glue->setOutputFile([outputFiles objectAtIndex:index]);
                }
                else {
                    glue->setOutputFile(tempFile);
                    if ((cpfFiles) && ([cpfFiles count] > 0) && batchMode) 
                    {
                        glue->setBuildCPF();
                        glue->setOutputFile(@"");
                    }
                    else 
                    {
                        glue->setOutputFile(tempFile);
                    }
                    
                }
                glue->setImage(file);
                validateRetVal = glue->dng_validate();
                if (!outputFiles && batchMode && (validateRetVal == dng_error_none))
                {
                    glue->saveCPFFile([cpfFiles objectAtIndex:index]);
                }
                
            }
        }
        index++;
    }
}


- (void) RunWorkerCompleted:(id) __unused param
{
	[NSApp endSheet:progressSheet];
	if ((validateRetVal != dng_error_none) && (validateRetVal != CornerFix_unknown_model) && (validateRetVal != CornerFix_saturated)
		&& (validateRetVal != CornerFix_bad_Bayer) && (validateRetVal != CornerFix_bad_linear) && (validateRetVal != CornerFix_bad_pixel_format)){
			
		NSString *s;
		
		switch (validateRetVal) {
			
			case dng_error_unknown:       			
				s = @"Logic or program error or other unclassifiable error.";
				break;
			case dng_error_not_yet_implemented:				
				s = @"Functionality requested is not yet implemented.";
				break;
			case dng_error_silent:							
				s = @"An error which should not be signalled to user.";
				break;
			case dng_error_user_canceled:					
				s = @"Processing stopped by user (or host application) request.";
				break;
			case dng_error_host_insufficient:				
				s = @"Necessary host functionality is not present.";
				break;
			case dng_error_memory:							
				s = @"Out of memory.";
				break;
			case dng_error_bad_format:						
				s = @"File format is not valid.";
				break;
			case dng_error_matrix_math:						
				s = @"Matrix has wrong shape, is badly conditioned: or similar problem.";
				break;
			case dng_error_open_file:						
				s = @"Could not open file.";
				break;
			case dng_error_read_file:						
				s = @"Error reading file.";
				break;
			case dng_error_write_file:						
				s = @"Error writing file.";
				break;
			case dng_error_end_of_file:					
				s = @"Unexpected end of file.";
				break;
			default: s = @"";
				
		}
		
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		//		[alert addButtonWithTitle:@"Cancel"];
		[alert setMessageText:@"DNG processing error"];
		[alert setInformativeText:s];
		[alert setAlertStyle:NSWarningAlertStyle];
		
		if ([alert runModal] == NSAlertFirstButtonReturn) {
			// OK clicked
		}
		[alert release];
	
	}
	else
		{
			
		if ((validateRetVal == CornerFix_unknown_model) || (validateRetVal == CornerFix_bad_Bayer) ||
			(validateRetVal == CornerFix_bad_linear) || (validateRetVal == CornerFix_bad_pixel_format))
		{
			NSAlert *alert = [[NSAlert alloc] init];
			[alert addButtonWithTitle:@"OK"];
			
			switch (validateRetVal) {
				case CornerFix_unknown_model:       			
					[alert setMessageText:@"Warning - Unsupported DNG format"];
					[alert setInformativeText:@"Your mileage may vary...."];
					break;
				case CornerFix_bad_Bayer:       			
					[alert setMessageText:@"Warning - CornerFix doesn't recognize the Bayer pattern"];
					[alert setInformativeText:@"You should try to convert to a Linear (demosaiced) DNG - see the CornerFix web site"];
				break;
				case CornerFix_bad_linear:       			
					[alert setMessageText:@"Warning - CornerFix doesn't recognize the data format"];
					[alert setInformativeText:@"You should try to convert to a Linear (demosaiced) DNG - see the CornerFix web site"];
				break;
				case CornerFix_bad_pixel_format:       			
					[alert setMessageText:@"Warning - CornerFix doesn't recognize the pixel format"];
					[alert setInformativeText:@"Your mileage may vary...."];
				break;
			}

			[alert setAlertStyle:NSWarningAlertStyle];
						
			[alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:nil];
			
			sheetNotDone = YES;
			while (sheetNotDone) {
				NSEvent *nextEvent = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate dateWithTimeIntervalSinceNow:0.05] inMode:NSModalPanelRunLoopMode dequeue:YES];
				if (nextEvent) {
					[NSApp sendEvent:nextEvent];
				}
			}
			
			[alert release];
			
		}
		else if (validateRetVal == CornerFix_saturated)
		{
			NSAlert *alert = [[NSAlert alloc] init];
			[alert addButtonWithTitle:@"OK"];
			//		[alert addButtonWithTitle:@"Cancel"];
			[alert setMessageText:@"Warning - Reference image contains saturated highlights"];
			[alert setInformativeText:@"Your profile may not be accurate"];
			[alert setAlertStyle:NSWarningAlertStyle];
			
			if ([alert runModal] == NSAlertFirstButtonReturn) {
				// OK clicked
			}
			[alert release];
			
		}
		
		if (!batchMode) {
			if (imageFiles) {
				if (([fileManager fileExistsAtPath:tempFile]) && ([fileManager isReadableFileAtPath:tempFile]))
				{
					NSImage *image = [[[ThumbNSImage alloc]  
						preferredInitFromFilename:tempFile 
						minSize: NSMakePoint(1024,  768)
						thumbPreferred: [[NSUserDefaults standardUserDefaults] boolForKey:@"preferThumbNails"]]autorelease];
					 
					if (!image || (NSEqualSizes([image size], NSZeroSize) == YES)) {
						image = [thumbnailWrapper thumbnailFromFilename:tempFile];
					}
					[correctedImage setImage:image];
				}
				else {
					[correctedImage setImage:NULL];
				}
			}
			else {
				[correctedImage setImage:NULL];
			}
		}
	}

	if (batchMode) {
		imageFiles = nil;
		outputFiles = nil;
		batchMode = false;
		if ((cpfFiles) && ([cpfFiles count] > 0)) {
			cpfFiles = nil;
			glue->resetCPF();
		}
		else
		{
			cpfFiles = loadedCPFFiles;
		}			
	}

	[self updateDisplay];
	[sheetProgress setDoubleValue:0];
	progressState = -1;
	disableMenu = NO;
	[mainMenu update];
}


- (void) runWorkerAsync:(id) __unused param
{
	disableMenu = YES;
#if defined(MULTITHREAD)
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#else	
#endif
	[self backgroundWorker1_DoWork];
#if defined(MULTITHREAD)
	// A piece of magic to get this to work when we're multi-threaded....... 
	[(self) performSelectorOnMainThread:@selector(RunWorkerCompleted:) withObject:nil waitUntilDone:NO];
	[pool release];
#else	
	[self RunWorkerCompleted:nil];
#endif
	
}

- (void) processImages {
	if (glue) {
		[self showProgressSheet];
		[sheetStatusField setStringValue:@"Processing....."];
//		[sheetProgress startAnimation:self];
		 // Get rid of the current image - it's the old one....
		[correctedImage setImage:nil];
//#define genCSVFile
#ifdef genCSVFile
	#pragma message("WARNING - Compiling CSV Options in")
		glue->setCSVFile([[[imageFiles objectAtIndex:0] stringByDeletingPathExtension] stringByAppendingString:@".csv"]);
		glue->setTIFFile([[[imageFiles objectAtIndex:0] stringByDeletingPathExtension] stringByAppendingString:@".tif"]);
#endif
		 glue->setVerbose([[NSUserDefaults standardUserDefaults] boolForKey:@"verboseMessages"]);
		 glue->setMultiple([[NSUserDefaults standardUserDefaults] boolForKey:@"multipleEquations"]);
		 glue->setDisableIlluminant(![[NSUserDefaults standardUserDefaults] boolForKey:@"luminanceCompensation"]);
		 glue->SetAntiAlias([[NSUserDefaults standardUserDefaults] integerForKey:@"antiAliasStrength"]);
		 glue->SetLumaStrength([[NSUserDefaults standardUserDefaults] integerForKey:@"luminance"]);
		 glue->SetChromaStrength([[NSUserDefaults standardUserDefaults] integerForKey:@"chroma"]);
		 glue->setCompress([[NSUserDefaults standardUserDefaults] boolForKey:@"levelCompression"]);
		 glue->setLossless([[NSUserDefaults standardUserDefaults] boolForKey:@"losslessCompression"]);
		 glue->setExifUpdate([[NSUserDefaults standardUserDefaults] boolForKey:@"updateEXIF"]);
		 glue->setCalculateAperture([[NSUserDefaults standardUserDefaults] boolForKey:@"calculateAperture"]);
		 glue->setClipApertures([[NSUserDefaults standardUserDefaults] boolForKey:@"clipAperture"]);
		 glue->setMaximizeResolution([[NSUserDefaults standardUserDefaults] boolForKey:@"maximizeResolution"]);
         glue->setOverwriteCameraName([[NSUserDefaults standardUserDefaults] boolForKey:@"appleDNGWorkaround"]);
         glue->setAsymetry([[NSUserDefaults standardUserDefaults] boolForKey:@"bidirectionalCorrection"] ? 1.0 : 500);
		
		 // Start the asynchronous operation.
		[sheetStatusField setStringValue:@"Processing........."];
	#if defined(MULTITHREAD)
		[NSThread detachNewThreadSelector:@selector(runWorkerAsync:) toTarget:self withObject:nil];
	#else
		 [self runWorkerAsync:nil];
	#endif
	 }
}


- (void)updateProgress:(NSNumber *) myNumber
{
	// This is a bit of a hack, but avoids passing untyped objects around.....
	 if (progressState != [myNumber intValue])
	 {
		 progressState = [myNumber intValue];
		 switch (progressState) {
			 case progressNone:
				 [sheetStatusField setStringValue:@"Ready....."];
				 [sheetProgress setDoubleValue:0];
				 break;
			 case progressReading:
				 [sheetStatusField setStringValue:@"Reading DNG....."];
				 [sheetProgress setDoubleValue:5];
				 break;
			 case progressDevignette:
				 [sheetStatusField setStringValue:@"Devignetting....."];
				 [sheetProgress setDoubleValue:10];
				 break;
			 case progressLinearization:
				 [sheetStatusField setStringValue:@"Linearizing....."];
				 [sheetProgress setDoubleValue:25];
				 break;
			 case progressIlluminant:
				 [sheetStatusField setStringValue:@"Illuminant compensation....."];
				 [sheetProgress setDoubleValue:45];
				 break;
			 case progressAnalysis:
				 [sheetStatusField setStringValue:@"Vignetting analysis....."];
				 [sheetProgress setDoubleValue:70];
				 break;
			 case progressInterpolate:
				 [sheetStatusField setStringValue:@"Interpolation....."];
				 [sheetProgress setDoubleValue:80];
				 break;
			 case progressRender:
				 [sheetStatusField setStringValue:@"Rendering....."];
				 [sheetProgress setDoubleValue:90];
				 break;
			 case progressWriteTIFF:
				 [sheetStatusField setStringValue:@"Writing TIFF....."];
				 [sheetProgress setDoubleValue:93];
				 break;
			 case progressThumbnail:
				 [sheetStatusField setStringValue:@"Building thumbnail....."];
				 [sheetProgress setDoubleValue:95];
				 break;
			 case progressWriteDNG:
				 [sheetStatusField setStringValue:@"Writing DNG....."];
				 [sheetProgress setDoubleValue:97];
				 break;
			 default:
				 [sheetStatusField setStringValue:@"Ready....."];
				 [sheetProgress setDoubleValue:0];
				 break;
		 }
	 }
	[sheetProgress display];
	
}


- (IBAction)openPreferencesWindow:(id) __unused sender
{
	[self prefsSignatureChanged];
	PrefsWindowController *pControl = (PrefsWindowController *) [PrefsWindowController sharedPrefsWindowController];
	[pControl setCallBack:self];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(prefWindowClose:)
												 name:@"NSWindowWillCloseNotification" object:[pControl window]];	
	[pControl showWindow:nil];
}


- (IBAction) openDocument:(id) __unused sender
{
    NSArray *fileTypes = [NSArray arrayWithObject:@"dng"];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	
    [oPanel setAllowsMultipleSelection:NO];
    if (imageDirectory) {
        [oPanel setDirectoryURL:[NSURL fileURLWithPath:imageDirectory]];
    }
    [oPanel setAllowedFileTypes:fileTypes];
    
    int result = [oPanel runModal];
    if (result == NSOKButton) {
        NSString* path = [[oPanel URL] path];
		if (([fileManager fileExistsAtPath:path]) && ([fileManager isReadableFileAtPath:path])) {
			[imageFiles release];
			imageFiles = nil;
			imageFiles = [[oPanel filenames] retain];
			[imageDirectory release];
			imageDirectory = nil;
			imageDirectory = [[path stringByDeletingLastPathComponent] retain];
			[outputFiles release];
			outputFiles = nil;
		}
		glue->setImage(path);
		[self updateDisplay];
		[self processImages];
    }
}	


- (IBAction) openProfile: __unused sender
{
    NSArray *fileTypes = [NSArray arrayWithObject:@"cpf"];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	
    [oPanel setAllowsMultipleSelection:NO];
    if (cpfDirectory) {
        [oPanel setDirectoryURL:[NSURL fileURLWithPath:cpfDirectory]];
    }

    [oPanel setAllowedFileTypes:fileTypes];
    
    int result = [oPanel runModal];
    if (result == NSOKButton) {
        NSString* path = [[oPanel URL] path];
		if (([fileManager fileExistsAtPath:path]) && ([fileManager isReadableFileAtPath:path])) {
			if (!glue->setCPFFile(path))
			{
				NSAlert *alert = [[NSAlert alloc] init];
				[alert addButtonWithTitle:@"OK"];
				//		[alert addButtonWithTitle:@"Cancel"];
				[alert setMessageText:@"That does not apper to be a valid lens profile"];
				[alert setInformativeText:@"Try again...."];
				[alert setAlertStyle:NSWarningAlertStyle];
				
				if ([alert runModal] == NSAlertFirstButtonReturn) {
					// OK clicked
				}
				[alert release];
			}
			[cpfFiles release];
			cpfFiles = [[oPanel filenames] retain];
			[cpfDirectory release];
			cpfDirectory = [[[oPanel filename] stringByDeletingLastPathComponent] retain];
			[self updateDisplay];
			[self processImages];
		}			
    }
}


- (IBAction) saveDocument: __unused sender
{
	if (imageFiles && ([imageFiles count] > 0)) {	
        int runResult;
		NSSavePanel *sp = [NSSavePanel savePanel];
		 
		/* set up new attributes */
        [sp setAllowedFileTypes:@[@"dng"]];
		 
		/* display the NSSavePanel */
		[outputFiles release];
		outputFiles = [[CornerFixFile renameFiles:imageFiles suffix:@"_CF" ] retain];
        
        if (imageDirectory) {
            [sp setDirectoryURL:[NSURL fileURLWithPath:imageDirectory]];
        }
        [sp setNameFieldStringValue:[[outputFiles objectAtIndex:0] lastPathComponent]];
        
		runResult = [sp runModal];
		[outputFiles release];
		outputFiles = nil;
		 
		/* if successful, save file under designated name */
		if (runResult == NSOKButton) {
			//Does this already exists?
            NSString* path = [[sp URL] path];
			if ([fileManager fileExistsAtPath:path]) {
				[fileManager removeItemAtPath:tempFile error:NULL];
			}
			if ([fileManager copyItemAtPath:tempFile toPath:path error:NULL]) {
				[imageDirectory release];
				imageDirectory = [[path stringByDeletingLastPathComponent] retain];
			}
			else {
				NSAlert *alert = [[NSAlert alloc] init];
				[alert addButtonWithTitle:@"OK"];
				//		[alert addButtonWithTitle:@"Cancel"];
				[alert setMessageText:@"Could not save file"];
				[alert setInformativeText:@"Could be a permissions problem - try running Disk Utility and repairing disk permissions"];
				[alert setAlertStyle:NSWarningAlertStyle];
				
				if ([alert runModal] == NSAlertFirstButtonReturn) {
					// OK clicked
				}
				[alert release];				
			}
		}
	}
	else {
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		//		[alert addButtonWithTitle:@"Cancel"];
		[alert setMessageText:@"No File is loaded"];
		[alert setInformativeText:@"You can't save a file till its loaded"];
		[alert setAlertStyle:NSWarningAlertStyle];
		
		if ([alert runModal] == NSAlertFirstButtonReturn) {
			// OK clicked
		}
		[alert release];
		
	}
  }

- (IBAction) createProfile: __unused sender
{
	 if (imageFiles) 
	 {
		 glue->setBuildCPF();
		 [cpfFiles release];
		 cpfFiles = [[CornerFixFile convertExtensions:imageFiles extension:@"cpf"] retain];
		 [self processImages];
		 [self updateDisplay];
	 }
	 else 
	 {
		 NSAlert *alert = [[NSAlert alloc] init];
		 [alert addButtonWithTitle:@"OK"];
		 //		[alert addButtonWithTitle:@"Cancel"];
		 [alert setMessageText:@"No reference image file loaded"];
		 [alert setInformativeText:@"First open an image file"];
		 [alert setAlertStyle:NSWarningAlertStyle];
		 
		 if ([alert runModal] == NSAlertFirstButtonReturn) {
			 // OK clicked
		 }
		 [alert release];		 
	 }
}


- (IBAction) saveProfile: __unused sender
{

	if (glue->getCPFState() != kcpfStateNone) {	
		
		NSSavePanel *sp;
		int runResult;
		
		/* create or get the shared instance of NSSavePanel */
		sp = [NSSavePanel savePanel];
		
		/* set up new attributes */
		//	[sp setAccessoryView:newView];
		[sp setAllowedFileTypes:[NSArray arrayWithObject:@"cpf"]];
		
		/* display the NSSavePanel */
        if (cpfDirectory) {
            [sp setDirectoryURL:[NSURL fileURLWithPath:cpfDirectory]];
        }

        [sp setNameFieldStringValue:[[cpfFiles objectAtIndex:0] lastPathComponent]];
        
		runResult = [sp runModal];
		
		/* if successful, save file under designated name */
		if (runResult == NSOKButton) {
			if (glue->saveCPFFile([sp filename])) {
				[cpfFiles release];
				cpfFiles = [[NSArray arrayWithObject:[[sp filename]retain]] retain];
				[cpfDirectory release];
				cpfDirectory = [[[sp filename] stringByDeletingLastPathComponent] retain];
				[self updateDisplay];
			}
			else {
				NSAlert *alert = [[NSAlert alloc] init];
				[alert addButtonWithTitle:@"OK"];
				//		[alert addButtonWithTitle:@"Cancel"];
				[alert setMessageText:@"Could not save file"];
				[alert setInformativeText:@"Could be a permissions problem - try running Disk Utility and repairing disk permissions"];
				[alert setAlertStyle:NSWarningAlertStyle];
				
				if ([alert runModal] == NSAlertFirstButtonReturn) {
					// OK clicked
				}
				[alert release];				
			}
		}
	}
	else {
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		//		[alert addButtonWithTitle:@"Cancel"];
		[alert setMessageText:@"No profile has been loaded or created"];
		[alert setInformativeText:@"You can't save a file till its loaded"];
		[alert setAlertStyle:NSWarningAlertStyle];
		
		if ([alert runModal] == NSAlertFirstButtonReturn) {
			// OK clicked
		}
		[alert release];
		
	}
	
}


- (IBAction) batchDocument: sender
{
	int result;
    NSArray *fileTypes = [NSArray arrayWithObject:@"dng"];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	
    [oPanel setAllowsMultipleSelection:YES];
    if (imageDirectory) {
        [oPanel setDirectoryURL:[NSURL fileURLWithPath:imageDirectory]];
    }
    [oPanel setAllowedFileTypes:fileTypes];
    
    result = [oPanel runModal];

    if (result == NSOKButton) {
		[originalImage setImage:nil];
		[correctedImage setImage:nil];

        NSArray *filesToOpen = [oPanel filenames];
		[imageFiles release];
		imageFiles = [filesToOpen retain];				
		[outputFiles release];
		outputFiles = [[CornerFixFile renameFiles:imageFiles suffix:@"_CF" ] retain];
		[loadedCPFFiles release];
		loadedCPFFiles = [cpfFiles retain];
		cpfFiles = nil;
		batchMode = true;
		[self processImages];			
    }	
}


- (IBAction) batchProfile: sender
{
	int result;
    NSArray *fileTypes = [NSArray arrayWithObject:@"dng"];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	
    [oPanel setAllowsMultipleSelection:YES];
    if (cpfDirectory) {
        [oPanel setDirectoryURL:[NSURL fileURLWithPath:cpfDirectory]];
    }
    [oPanel setAllowedFileTypes:fileTypes];
    
    result = [oPanel runModal];

    if (result == NSOKButton) {
		[originalImage setImage:nil];
		[correctedImage setImage:nil];
		
        NSArray *filesToOpen = [oPanel filenames];
		[imageFiles release];
		imageFiles = [filesToOpen retain];				
		[outputFiles release];
		outputFiles = nil;
		[cpfFiles release];
		cpfFiles = [[CornerFixFile convertExtensions:imageFiles extension:@"cpf" ] retain];
		batchMode = true;
		[self processImages];			
    }
}

- (IBAction) imageApplyHandler: sender
{
	[self prefsSignatureChanged];
	[self processImages];				
}

- (void)prefWindowClose:(NSNotification *)notification
{
	if ([self prefsSignatureChanged]) 
	{
		[self processImages];
	}
}


- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
//    if ([keyPath isEqual:@"antiAliasStrength"]) {
//		NSLog(@"observeValueForKeyPath hit");
//	}
	[self processImages];
    // We dont have a real superclass, so no need to call super
}

- (IBAction)showHelp:(id)sender
{
	if (![[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://sites.google.com/site/cornerfix/using-cornerfix"]]) 
	{
		NSLog(@"Open of http://sites.google.com/site/cornerfix/using-cornerfix failed.");
	}
	
}


@end
