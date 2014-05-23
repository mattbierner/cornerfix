/*!
	@header		ApertureEditPlugIn.h	
 */

/*Copyright 2008-2013 Apple Inc.  All rights reserved.*/


#import <PluginManager/PROAPIAccessing.h>
#import "ApertureSDKCommon.h"
#import "ApertureEditManager.h"

/*!
	@protocol		ApertureEditPlugIn
	@discussion		Protocol definition for an Aperture Edit plug-in session.
 */
@protocol ApertureEditPlugIn

/*!
	@abstract		Brokers version management between a plug-in and the host application.
	@param			apiManager The ProPlug plug-in manager object.
	@result			id An initialized plug-in controller object.
	@discussion		The apiManager object is the protocol broker between a plug-in and the host. It 
					ensures that a plug-in supporting a particular version of the API is given the host 
					objects that correspond to this version. A plug-in should call -apiForProtocol on 
					the apiManager object to obtain a reference to the host edit manager for use 
					throughout the editing process. If the plug-in fails to obtain a reference to the 
					host edit manager, it should fail to initialize. 
 */
- (id)initWithAPIManager:(id<PROAPIAccessing>)apiManager;

/*!	
	@abstract		A callback for the Edit manager's import method.
	@param			editManager
	@param			path
	@param			versionUniqueID	
	@discussion		Aperture calls this method after the successful completion of an import. If a plug-in does 
   request imports, it should not end or cancel the edit session until it receives one of the import callbacks.
*/
- (void)editManager:(id<ApertureEditManager>)editManager didImportImageAtPath:(NSString *)path versionUniqueID:(NSString *)versionUniqueID;


/*!
	@abstract		A callback for the Edit manager's import method.
	@param			editManager
	@param			path
	@param			error
	@discussion		Aperture calls this method after an import failure. If your plug-in does 
   request imports, it should not end or cancel the edit session until it receives one of the import callbacks.
*/
- (void)editManager:(id<ApertureEditManager>)editManager didNotImportImageAtPath:(NSString *)path error:(NSError *)error;

/*!		
	@abstract		Notifies a plug-in that Aperture is ready for editing.
	@discussion		Notification can take place even though Aperture has not yet asked for the plug-in's UI. The plug-in can begin making calls, getting
   properties, and so on. 
*/
- (void)beginEditSession;

/*!
	@abstract		Returns the window that contains the plug-in's entire UI.
	@result			An NSWindow. Returning nil indicates an error condition. Aperture deallocates the plug-in.
	@discussion		Aperture places this window on screen, centers it over the Aperture window, and runs it modally.
   
*/
- (NSWindow *)editWindow;


@end
