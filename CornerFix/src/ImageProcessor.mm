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
* CornerFixManager.mm
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

#import "ImageProcessor.h"

#include "dng_local_printf.h"
#include "dng_win_glue.h"
#include "dng_errors.h"
#include "CornerFixVersion.h"
#include "CornerFixFile.h"

#include <unistd.h>



@implementation ImageProcessor

- (dng_win_glue*) configureGlue: (dng_win_glue*) g
{
     g->setVerbose([[NSUserDefaults standardUserDefaults] boolForKey:@"verboseMessages"]);
     g->setMultiple([[NSUserDefaults standardUserDefaults] boolForKey:@"multipleEquations"]);
     g->setDisableIlluminant(![[NSUserDefaults standardUserDefaults] boolForKey:@"luminanceCompensation"]);
     g->SetAntiAlias([[NSUserDefaults standardUserDefaults] integerForKey:@"antiAliasStrength"]);
     g->SetLumaStrength([[NSUserDefaults standardUserDefaults] integerForKey:@"luminance"]);
     g->SetChromaStrength([[NSUserDefaults standardUserDefaults] integerForKey:@"chroma"]);
     g->setCompress([[NSUserDefaults standardUserDefaults] boolForKey:@"levelCompression"]);
     g->setLossless([[NSUserDefaults standardUserDefaults] boolForKey:@"losslessCompression"]);
     g->setExifUpdate([[NSUserDefaults standardUserDefaults] boolForKey:@"updateEXIF"]);
     g->setCalculateAperture([[NSUserDefaults standardUserDefaults] boolForKey:@"calculateAperture"]);
     g->setClipApertures([[NSUserDefaults standardUserDefaults] boolForKey:@"clipAperture"]);
     g->setMaximizeResolution([[NSUserDefaults standardUserDefaults] boolForKey:@"maximizeResolution"]);
     g->setOverwriteCameraName([[NSUserDefaults standardUserDefaults] boolForKey:@"appleDNGWorkaround"]);
     g->setAsymetry([[NSUserDefaults standardUserDefaults] boolForKey:@"bidirectionalCorrection"] ? 1.0 : 500);
		
    return g;
}

+ (id) processorForProfile:(NSString *)profile
{
    ImageProcessor* processor = [[ImageProcessor alloc] init];
    [processor loadProfile:profile];
    return processor;
}

- (id) init
{
	glue = new dng_win_glue();
	tempFile = nil;
	loadedCPFFiles = nil;
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
	
	/*if (!tempFile) {
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
		//[NSApp terminate:nil];

	}	*/
	
	// Now we need to register the various defaults for the preferences 
		
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
		
		/*NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		//		[alert addButtonWithTitle:@"Cancel"];
		[alert setMessageText:@"DNG processing error"];
		[alert setInformativeText:s];
		[alert setAlertStyle:NSWarningAlertStyle];
		
		if ([alert runModal] == NSAlertFirstButtonReturn) {
			// OK clicked
		}
		[alert release];*/
	
	}
	else
		{
			
		if ((validateRetVal == CornerFix_unknown_model) || (validateRetVal == CornerFix_bad_Bayer) ||
			(validateRetVal == CornerFix_bad_linear) || (validateRetVal == CornerFix_bad_pixel_format))
		{
			
			/*NSAlert *alert = [[NSAlert alloc] init];
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
			*/
			sheetNotDone = YES;
			while (sheetNotDone) {
				//NSEvent *nextEvent = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate dateWithTimeIntervalSinceNow:0.05] inMode:NSModalPanelRunLoopMode dequeue:YES];
				//if (nextEvent) {
				//	[NSApp sendEvent:nextEvent];
				//}
			}
			
			//[alert release];
			
		}
		else if (validateRetVal == CornerFix_saturated)
		{
			/*NSAlert *alert = [[NSAlert alloc] init];
			[alert addButtonWithTitle:@"OK"];
			//		[alert addButtonWithTitle:@"Cancel"];
			[alert setMessageText:@"Warning - Reference image contains saturated highlights"];
			[alert setInformativeText:@"Your profile may not be accurate"];
			[alert setAlertStyle:NSWarningAlertStyle];
			
			if ([alert runModal] == NSAlertFirstButtonReturn) {
				// OK clicked
			}
			[alert release];*/
			
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

//	[sheetProgress setDoubleValue:0];
	progressState = -1;
	disableMenu = NO;
	//[mainMenu update];
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
/*
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
*/

- (void) processFile:(NSString*)file
    ok:(void(^)())ok
    err:(void(^)())err
{
    if ([fileManager isReadableFileAtPath:file]) {
        glue->setImage(file);
        glue->setOutputFile([CornerFixFile renameFile:file suffix:@"_CF"]);
        validateRetVal = glue->dng_validate();
    }

    ok();
}

- (void) processFiles:(NSArray*)files
    ok:(void(^)())ok
    err:(void(^)())err
{
    for (NSString* file in files)
        [self processFile:file ok:ok err:err];
}

/*
- (IBAction) saveDocument: __unused sender
{
	if (imageFiles && ([imageFiles count] > 0)) {	
        int runResult;
		NSSavePanel *sp = [NSSavePanel savePanel];
		 
		// set up new attributes
        [sp setAllowedFileTypes:@[@"dng"]];
		 
		// display the NSSavePanel
		[outputFiles release];
		outputFiles = [[self renameFiles:imageFiles suffix:@"_CF" ] retain];
        
        if (imageDirectory) {
            [sp setDirectoryURL:[NSURL fileURLWithPath:imageDirectory]];
        }
        [sp setNameFieldStringValue:[[outputFiles objectAtIndex:0] lastPathComponent]];
        
		runResult = [sp runModal];
		[outputFiles release];
		outputFiles = nil;
		 
		// if successful, save file under designated name
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
*/
/*
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
		//[originalImage setImage:nil];
		//[correctedImage setImage:nil];

        NSArray *filesToOpen = [oPanel filenames];
		[imageFiles release];
		imageFiles = [filesToOpen retain];				
		[outputFiles release];
		outputFiles = [[self renameFiles:imageFiles suffix:@"_CF" ] retain];
		[loadedCPFFiles release];
		loadedCPFFiles = [cpfFiles retain];
		cpfFiles = nil;
		batchMode = true;
		[self processImages];			
    }	
}*/

- (void) loadProfile:(NSString *)file
{
    if ([fileManager isReadableFileAtPath:file]) {
        if (!glue->setCPFFile(file))
        {
            /*NSAlert *alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle:@"OK"];
            [alert setMessageText:@"That does not apper to be a valid lens profile"];
            [alert setInformativeText:@"Try again...."];
            [alert setAlertStyle:NSWarningAlertStyle];
            
            if ([alert runModal] == NSAlertFirstButtonReturn) {
                // OK clicked
            }
            [alert release];*/
        }
        [self configureGlue:glue];
    }
}

@end
