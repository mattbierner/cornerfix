//
//	CornerFixAperatureEditPlugin.m
//	CornerFixAperature
//
//	Created by Matt Bierner on 5/21/14.
//	  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
//

#import "CornerFixAperatureEditPlugin.h"


@implementation CornerFixAperatureEditPlugIn

//---------------------------------------------------------
// initWithAPIManager:
//
// This method is called when a plug-in is first loaded, and
// is a good point to conduct any checks for anti-piracy or
// system compatibility. This is also your only chance to
// obtain a reference to Aperture's export manager. If you
// do not obtain a valid reference, you should return nil.
// Returning nil means that a plug-in chooses not to be accessible.
//---------------------------------------------------------

- (id)initWithAPIManager:(id<PROAPIAccessing>)apiManager
{
	if (self = [super init])
	{
		_apiManager	= apiManager;
		_editManager = [[_apiManager apiForProtocol:@protocol(ApertureEditManager)] retain];
		if (!_editManager)
			return nil;
				
		// Finish your initialization here
	}
	
	return self;
}

- (void)dealloc
{
	[_topLevelNibObjects release];

	[_editManager release];
	
	[super dealloc];
}


#pragma mark -
// UI Methods
#pragma mark UI Methods

- (NSWindow *)editWindow
{
	if (_editWindow == nil)
	{
		NSBundle *myBundle = [NSBundle bundleForClass:[self class]];
		NSNib *myNib = [[NSNib alloc] initWithNibNamed:@"CornerFixAperature" bundle:myBundle];
		if ([myNib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)])
		{
			if ([myNib instantiateWithOwner:self topLevelObjects:&_topLevelNibObjects])
			{
				[_topLevelNibObjects retain];
			}
		}
		else
		{
			if ([myNib instantiateNibWithOwner:self topLevelObjects:&_topLevelNibObjects])
			{
				[_topLevelNibObjects retain];
			}
		}
		[myNib release];
	}
	
	return _editWindow;
}

#pragma mark -
// Edit Session Methods
#pragma mark Edit Session Methods

- (void)beginEditSession
{
	
}

- (void)editManager:(id<ApertureEditManager>)editManager didImportImageAtPath:(NSString *)path versionUniqueID:(NSString *)versionUniqueID
{
	
}

- (void)editManager:(id<ApertureEditManager>)editManager didNotImportImageAtPath:(NSString *)path error:(NSError *)error;
{
	
}

@end
