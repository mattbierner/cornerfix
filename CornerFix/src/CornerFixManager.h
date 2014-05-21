#include <Foundation/Foundation.h>

class dng_win_glue;
@class ImageProcessor;


@interface CornerFixManager : NSObject
{
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
	dng_win_glue * glue;
	NSFileManager * fileManager;
	bool disableMenu;
	long long prefsSignature;
    bool sheetNotDone;
    
    ImageProcessor* processor;
}

- (void) runWorkerAsync:(id)param;
- (void) RunWorkerCompleted:(id)param;

- (BOOL) prefsSignatureChanged;

- (dng_win_glue*) configureGlue:(dng_win_glue*) glue;

- (void) processFile:(NSString*)file
    ok:(void(^)())ok
    err:(void(^)())err;

- (void) createProfile:(NSString*) file
    ok:(void(^)()) ok
    err:(void(^)()) err;

- (void) createProfiles:(NSArray*) files
    ok:(void(^)()) ok
    err:(void(^)()) err;


  // File handling
  /*
- (IBAction) openDocument: sender;
- (IBAction) saveDocument: sender;
- (IBAction) batchDocument: sender;
- (IBAction) openPreferencesWindow:(id)sender;
- (IBAction) openProfile: sender;

- (IBAction) saveProfile: sender;
- (IBAction) batchProfile: sender;
- (IBAction) imageApplyHandler: sender;
- (void)prefWindowClose:(NSNotification *)notification;
- (void)updateProgress:(NSNumber *) myNumber;
*/
@end
