/**
 *
 */
#include <Foundation/Foundation.h>
#include <memory>

class dng_win_glue;
@class CornerFixConfiguration;

typedef void(^process_file_success)();
typedef void(^process_file_failure)();


@interface ImageProcessor : NSObject
{
	std::shared_ptr<dng_win_glue> glue;
	NSFileManager* fileManager;
}

+ (id) processorForProfile:(NSString*) profile
    config:(CornerFixConfiguration*) config
    error:(NSError**) error;

- (id) init;

- (id) initWithGlue:(std::shared_ptr<dng_win_glue>) glue;

- (BOOL) loadConfiguration:(CornerFixConfiguration*) config
    error:(NSError**) error;

- (BOOL) loadProfile:(NSString*) profile
    error:(NSError**) error;

- (void) processFile:(NSString*)input
    output:(NSString*) output
    ok:(process_file_success) ok
    err:(process_file_failure) err;

- (void) processFiles:(NSArray*)files
    ok:(process_file_success) ok
    err:(process_file_failure) err;

@end
