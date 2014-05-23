#import "ImageProcessor.h"

#include "dng_local_printf.h"
#include "dng_win_glue.h"
#include "dng_errors.h"

#include "CornerFixVersion.h"
#include "CornerFixFile.h"

#include <unistd.h>
#include <string>

@implementation ImageProcessor


+ (id) processorForProfile:(NSString *)profile
    config:(NSUserDefaults *)config
{
    ImageProcessor* processor = [[ImageProcessor alloc] init];
    [processor loadConfiguration:config];
    [processor loadProfile:profile];
    return processor;
}

- (id) init
{
	glue = new dng_win_glue();

	fileManager = [NSFileManager defaultManager];
		
	dng_local_printf::setWorker(self);
	
	return self;
}

- (void) processFile:(NSString*)file
    output:(NSString*)output
    ok:(void(^)())ok
    err:(void(^)())err
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


- (void) processFiles:(NSArray*)files
    ok:(void(^)())ok
    err:(void(^)())err
{
    for (NSString* file in files)
        [self processFile:file output:[CornerFixFile renameFile:file suffix:@"_CF"] ok:ok err:err];
}


- (void) loadConfiguration:(NSUserDefaults *)options
{
    glue->setVerbose([options boolForKey:@"verboseMessages"]);
    glue->setMultiple([options boolForKey:@"multipleEquations"]);
    glue->setDisableIlluminant(![options boolForKey:@"luminanceCompensation"]);
    glue->SetAntiAlias([options integerForKey:@"antiAliasStrength"]);
    glue->SetLumaStrength([options integerForKey:@"luminance"]);
    glue->SetChromaStrength([options integerForKey:@"chroma"]);
    glue->setCompress([options boolForKey:@"levelCompression"]);
    glue->setLossless([options boolForKey:@"losslessCompression"]);
    glue->setExifUpdate([options boolForKey:@"updateEXIF"]);
    glue->setCalculateAperture([options boolForKey:@"calculateAperture"]);
    glue->setClipApertures([options boolForKey:@"clipAperture"]);
    glue->setMaximizeResolution([options boolForKey:@"maximizeResolution"]);
    glue->setOverwriteCameraName([options boolForKey:@"appleDNGWorkaround"]);
    glue->setAsymetry([options boolForKey:@"bidirectionalCorrection"] ? 1.0 : 500);
}


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
    }
}

@end
