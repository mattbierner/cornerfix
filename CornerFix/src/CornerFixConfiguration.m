#import "CornerFixConfiguration.h"

@implementation CornerFixConfiguration

@synthesize verboseMessages;
@synthesize multipleEquations;
@synthesize luminanceCompensation;
@synthesize levelCompression;
@synthesize losslessCompression;
@synthesize updateEXIF;
@synthesize calculateAperture;
@synthesize clipAperature;
@synthesize maximizeResolution;
@synthesize appleDNGWorkaround;
@synthesize bidirectionalCorrection;
@synthesize antiAliasStrength;
@synthesize luminance;
@synthesize chroma;

+ (CornerFixConfiguration*) defaults
{
    CornerFixConfiguration* config = [[CornerFixConfiguration alloc] init];
    
    config.verboseMessages = NO;
    config.multipleEquations = NO;
    config.luminanceCompensation = YES;
    config.levelCompression = NO;
    config.losslessCompression = NO;
    config.updateEXIF = NO;
    config.calculateAperture = YES;
    config.clipAperature = YES;
    config.maximizeResolution = NO;
    config.appleDNGWorkaround = NO;
    config.bidirectionalCorrection = YES;
    
    config.antiAliasStrength = 0;
    config.luminance = 100;
    config.chroma = 100;
    return [config autorelease];
}

+ (CornerFixConfiguration*) fromUserDefaults:(NSUserDefaults*)options
{
    CornerFixConfiguration* config = [[CornerFixConfiguration alloc] init];
    
    config.verboseMessages = [options boolForKey:@"verboseMessages"];
    config.multipleEquations = [options boolForKey:@"multipleEquations"];
    config.luminanceCompensation = [options boolForKey:@"luminanceCompensation"];
    config.levelCompression = [options boolForKey:@"levelCompression"];
    config.losslessCompression = [options boolForKey:@"losslessCompression"];
    config.updateEXIF = [options boolForKey:@"updateEXIF"];
    config.calculateAperture = [options boolForKey:@"calculateAperture"];
    config.clipAperature = [options boolForKey:@"clipAperture"];
    config.maximizeResolution = [options boolForKey:@"maximizeResolution"];
    config.appleDNGWorkaround = [options boolForKey:@"appleDNGWorkaround"];
    config.bidirectionalCorrection = [options boolForKey:@"bidirectionalCorrection"];
    
    config.antiAliasStrength = [options integerForKey:@"antiAliasStrength"];
    config.luminance = [options integerForKey:@"luminance"];
    config.chroma = [options integerForKey:@"chroma"];

    return [config autorelease];
}


@end
