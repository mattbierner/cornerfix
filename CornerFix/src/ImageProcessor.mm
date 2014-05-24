#import "ImageProcessor.h"

#include "dng_win_glue.h"
#include "dng_local_printf.h"
#include "dng_errors.h"

#include "CornerFixFile.h"
#include "CornerFixConfiguration.h"

#include <string>

@implementation ImageProcessor

+ (id) processorForProfile:(NSString*)profile
    config:(CornerFixConfiguration*)config
    error:(NSError**) error
{
    ImageProcessor* processor = [[ImageProcessor alloc] init];
    
    if (![processor loadConfiguration:config error:error])
        return nil;
    if (![processor loadProfile:profile error:error])
        return nil;
    
    return [processor autorelease];
}

- (id) init
{
	glue = new dng_win_glue();

	fileManager = [NSFileManager defaultManager];
		
	dng_local_printf::setWorker(self);
	
	return self;
}


- (BOOL) loadConfiguration:(CornerFixConfiguration*)config error:(NSError**) __unused error
{
    glue->setVerbose(config.verboseMessages);
    glue->setMultiple(config.multipleEquations);
    glue->setDisableIlluminant(!config.luminanceCompensation);
    glue->SetAntiAlias((int)config.antiAliasStrength);
    glue->SetLumaStrength((int)config.luminance);
    glue->SetChromaStrength((int)config.chroma);
    glue->setCompress(config.levelCompression);
    glue->setLossless(config.losslessCompression);
    glue->setExifUpdate(config.updateEXIF);
    glue->setCalculateAperture(config.calculateAperture);
    glue->setClipApertures(config.clipAperature);
    glue->setMaximizeResolution(config.maximizeResolution);
    glue->setOverwriteCameraName(config.appleDNGWorkaround);
    glue->setAsymetry(config.bidirectionalCorrection ? 1.0 : 500);
    
    return YES;
}

- (BOOL) loadProfile:(NSString *)file error:(NSError**) error
{
    if (![fileManager isReadableFileAtPath:file]) {
        *error = [NSError
            errorWithDomain:@"cornerfix"
            code:dng_error_read_file
            userInfo:[NSDictionary
                dictionaryWithObject:[NSString stringWithFormat:@"Could not read profile:%@", file]
                forKey:NSLocalizedDescriptionKey]];
        return NO;
    }
    
    if (!glue->setCPFFile(file))
    {
        *error = [NSError
            errorWithDomain:@"cornerfix"
            code:dng_error_read_file
            userInfo:[NSDictionary
                dictionaryWithObject:[NSString stringWithFormat:@"Could not processes profile:%@", file]
                forKey:NSLocalizedDescriptionKey]];
        return NO;
    }
    
    return YES;
}


- (void) processFile:(NSString*) file
    output:(NSString*) output
    ok:(process_file_success) ok
    err:(process_file_failure) err
{
    if (![fileManager isReadableFileAtPath:file])
        err();
    
    glue->setImage(file);
    glue->setOutputFile(output);
    int validateRetVal = glue->dng_validate();
    if (validateRetVal != dng_error_none)
        err();

    ok();
}


- (void) processFiles:(NSArray*) files
    ok:(process_file_success) ok
    err:(process_file_failure) err
{
    for (NSString* file in files)
        [self processFile:file output:[CornerFixFile renameFile:file suffix:@"_CF"] ok:ok err:err];
}

@end
