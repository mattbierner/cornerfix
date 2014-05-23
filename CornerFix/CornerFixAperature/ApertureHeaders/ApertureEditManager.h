/*!
	@header			ApertureEditManager.h
*/

/* Copyright 2008-2013 Apple Inc.  All rights reserved. */

#import "ApertureSDKCommon.h"

/* New in Aperture 2 - Allows edit plug-ins to specify the format of new editable versions */
typedef enum
{	
	kApertureImageFormatTIFF8 = 2,
	kApertureImageFormatTIFF16 = 3,
	kApertureImageFormatPSD16 = 4,
	kApertureImageFormatPSD8 = 5,
} ApertureImageFormat;

/*!
	@protocol		ApertureEditManager
	@discussion		Protocol definition for the Aperture Edit interface. You should only call these methods from the main thread.
 */
@protocol ApertureEditManager

/*!	
	@abstract		Returns the unique ids of the versions the user selected in Aperture.
	@result			An NSArray.
	@discussion		The IDs are ordered in the same order that the user selected them, except the primary selection is always first.
*/
- (NSArray *)selectedVersionIds;

/*!	
	@abstract		Returns the unique ids that the plug-in can edit.
	@result			An NSArray.
	@discussion		The array may include some images the user selected if they were already marked as editable.
*/
- (NSArray *)editableVersionIds;

/*!	
	@abstract		Returns the unique ids of images the plug-in has imported during the session.
	@result			An NSArray.
*/
- (NSArray *)importedVersionIds;

/*!	
	@abstract		Accesses all the available properties for the specified image.
	@param			versionUniqueID
	@result			An NSDictionary
	@discussion		A thumbnail image is not included. You can obtain a thumbnail using the method -thumbnailForVersion:.
*/
- (NSDictionary *)propertiesWithoutThumbnailForVersion:(NSString *)versionUniqueID;

/*!	
	@abstract		Returns the thumbnail for an image.
	@param			versionUniqueID
	@param			size
	@result			An NSImage.
	@discussion		Some plug-ins may choose to operate on the large thumbnail and not request an editable version
   of the image until the user is done.
*/
- (NSImage *)thumbnailForVersion:(NSString *)versionUniqueID size:(ApertureExportThumbnailSize)size;

/*!	
	@abstract		Returns the path to the file that the plug-in should read/write for this version.
	@param			versionUniqueID
	@result			An NSString
	@discussion		This path is also available via the properties dictionary, but if this version is not editable, the method returns nil.
*/
- (NSString *)pathOfEditableFileForVersion:(NSString *)versionUniqueID;

/*!
	@abstract		Returns an array of unique IDs.
	@param			versionUniqueIDs
	@param			stackWithOriginal
	@result			An NSArray. Each entry in the array contains the properties dictionary for the editable version.
	@discussion		If an image was already editable and the user did not want to guarantee a copy (by holding the option key when
   selecting the plug-in from the menu), then this method returns the unique ID passed in for that version. Otherwise, this method tells
   Aperture to write out an editable image in the format specified in the user's preferences (PSD or TIFF) and creates
   a new entry in the user's library. Normally, the plug-in then calls -pathOfEditableFileForVersion: for the unique IDs returned from
   this method, or the -propertiesWithoutThumbnailForVersion: and -thumbnailForVersion: methods.
*/
- (NSArray *)editableVersionsOfVersions:(NSArray *)versionUniqueIDs stackWithOriginal:(BOOL)stackWithOriginal;

/*!
	@abstract		Returns an array of unique IDs.
	@param			versionUniqueIDs
	@param			format
	@param			stackWithOriginal
	@result			An NSArray. Each entry in the array contains the properties dictionary for the editable version.
	@discussion		If an image was already editable and the user did not want to guarantee a copy (by holding the option key when
   selecting the plug-in from the menu) then this method will return the unique ID passed in for that version. Otherwise, this method tells
   Aperture to write out an editable image in the format specified by format and creates
   a new entry in the user's library. Normally, the plug-in would then call the -pathOfEditableFileForVersion: for the unique IDs returned from
   this method, or the -propertiesWithoutThumbnailForVersion: and -thumbnailForVersion: methods.
*/
- (NSArray *)editableVersionsOfVersions:(NSArray *)versionUniqueIDs requestedFormat:(ApertureImageFormat)format stackWithOriginal:(BOOL)stackWithOriginal;

/*!		
	@abstract		Returns YES if Aperture allows the plug-in to import images into the current album; NO otherwise.
	@result			A Boolean.
	@discussion		For example, Aperture does not allow import into smart albums. 
*/
- (BOOL)canImport;

/*!
	@abstract		Asynchronously imports an image into the current album.
	@param			imagePath
	@param			isReferenced
	@param			versionUniqueIdsToStack		
	@discussion		This method calls the -editManager:didImportImageAtPath:versionUniqueID: method upon successful completion, or -editManager:didNotImportImageAtPath:error: if the import was unsuccessful. If -canImport returns NO, this method does nothing and the plug-in does not get a callback. The plug-in should not call -endEditSession or -cancelEditSession until it has received the import callback.
*/
- (void)importImageAtPath:(NSString *)imagePath referenced:(BOOL)isReferenced stackWithVersions:(NSArray *)versionUniqueIdsToStack;

/*!	
	@abstract		Deletes the specified versions and their master files from the user's library. 
	@param			versionUniqueIDs	
	@discussion		Aperture only performs this operation for versions created by the plug-in during the current session. Unique IDs for any images that were not created by the plug-in are ignored. This includes images that were already editable that the plug-in has modified. Note this call will delete master files for editable versions, whether those masters are referenced or not. However, it will only delete master files of imported versions if those files are managed (not referenced).
*/
- (void)deleteVersions:(NSArray *)versionUniqueIDs;

/*!
	@abstract		Adds the specified key-value pairs to the Custom Metadata for this image.
	@param			customMetadata
	@param			versionUniqueIDs		
	@discussion		This data appears in the "Other" tab in the metadata inspector. If an image already has a value for the specified key, it is updated to the new value
*/
- (void)addCustomMetadata:(NSDictionary *)customMetadata toVersions:(NSArray *)versionUniqueIDs;

/*!	
	@abstract		Pass in an array of arrays specifying a keyword hierarchy.
	@param			hierarchicalKeywords
	@param			versionUniqueIDs	
*/
- (void)addHierarchicalKeywords:(NSArray *)hierarchicalKeywords toVersions:(NSArray *)versionUniqueIDs;

/*!	
	@abstract		Returns Aperture's main window
	@result			An NSWindow.
	@discussion		The Edit plug-in uses this method in case it needs to present a sheet.
*/
- (NSWindow *)apertureWindow;

/*!
	@abstract		Notifies Aperture that the plug-in has completed its work.	
	@discussion		The plug-in should be ready to dealloc at the time of this call and should not be running tasks on any other threads, or anything on the run loop that may call back after this call is finished. 
*/
- (void)endEditSession;

/*!	
	@abstract		Similar to -endEditSession, but automatically deletes any editable versions created by the plug-in during this session.	
	@discussion		Note that this method does NOT delete any versions that were edited by the plug-in, but not created during this session. This call also does not delete any referenced master files imported by the plug-in.
*/
- (void)cancelEditSession;

/*!	
	@abstract		Sets the users preferences values.
	@param			value
	@param			key	
	@discussion		Writes preferences values into a preferences file specific to the plug-in, not Aperture's preferences file.
*/
- (void)setUserDefaultsValue:(id)value forKey:(NSString *)key;

/*!	
	@abstract		Reads the specified user preferences values.
	@param			key
	@result			id	
	@discussion		Reads the specified preferences values from a preferences file specific to the plug-in, not Aperture's preferences file.
*/
- (id)userDefaultsObjectForKey:(NSString *)key;

@end
