//
//	CornerFixAperatureEditPlugin.h
//	CornerFixAperature
//
//	Created by Matt Bierner on 5/21/14.
//	  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import "ApertureEditPlugIn.h"
#import "ApertureEditManager.h"


@interface CornerFixAperatureEditPlugIn : NSObject <ApertureEditPlugIn>
{
	// The cached API Manager object, as passed to the -initWithAPIManager: method.
	id _apiManager; 
	
	// The cached Aperture Export Manager object - you should fetch this from the API Manager during -initWithAPIManager:
	NSObject<ApertureEditManager, PROAPIObject> *_editManager; 
		
	// Top-level objects in the nib are automatically retained - this array
	// tracks those, and releases them
	NSArray *_topLevelNibObjects;
	
	// Outlets to your plug-ins user interface
	IBOutlet NSWindow *_editWindow;
}

@end
