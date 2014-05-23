/**
 *
 */
#include <Foundation/Foundation.h>

class dng_win_glue;

@interface ImageProcessor : NSObject
{
	dng_win_glue* glue;
	NSFileManager* fileManager;
}

+ (id) processorForProfile:(NSString*)profile
    config:(NSUserDefaults*) config;

- (id) init;

- (void) loadConfiguration:(NSUserDefaults*) config;
- (void) loadProfile:(NSString*) profile;

- (void) processFile:(NSString*)input
    output:(NSString*)output
    ok:(void(^)())ok
    err:(void(^)())err;

- (void) processFiles:(NSArray*)files
    ok:(void(^)())ok
    err:(void(^)())err;

@end
