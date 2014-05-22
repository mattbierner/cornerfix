#include <Foundation/Foundation.h>

class dng_win_glue;

@interface ImageProcessor : NSObject
{
	NSString * tempFile;
	NSArray * loadedCPFFiles;
	NSArray  * imageFiles;
	NSArray * cpfFiles;
	NSArray * outputFiles;
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
}

+ (id) processorForProfile:(NSString*)profile;

- (id) init;

- (void) loadProfile:(NSString*) profile;

- (void) runWorkerAsync:(id)param;
- (void) RunWorkerCompleted:(id)param;

- (dng_win_glue*) configureGlue:(dng_win_glue*) glue;

- (void) processFile:(NSString*)file
    ok:(void(^)())ok
    err:(void(^)())err;

- (void) processFiles:(NSArray*)files
    ok:(void(^)())ok
    err:(void(^)())err;

@end
