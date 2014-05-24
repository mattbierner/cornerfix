//
//  Configuration.h
//  CornerFix
//
//  Created by Matt Bierner on 5/23/14.
//  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CornerFixConfiguration : NSObject
{
    BOOL verboseMessages;
    BOOL multipleEquations;
    BOOL luminanceCompensation;
    BOOL levelCompression;
    BOOL losslessCompression;
    BOOL updateEXIF;
    BOOL calculateAperture;
    BOOL clipAperature;
    BOOL maximizeResolution;
    BOOL appleDNGWorkaround;
    BOOL bidirectionalCorrection;
    
    NSInteger antiAliasStrength;
    NSInteger luminance;
    NSInteger chroma;
}

+ (CornerFixConfiguration*) fromUserDefaults:(NSUserDefaults*)options;

@property (nonatomic) BOOL verboseMessages;
@property (nonatomic) BOOL multipleEquations;
@property (nonatomic) BOOL luminanceCompensation;
@property (nonatomic) BOOL levelCompression;
@property (nonatomic) BOOL losslessCompression;
@property (nonatomic) BOOL updateEXIF;
@property (nonatomic) BOOL calculateAperture;
@property (nonatomic) BOOL clipAperature;
@property (nonatomic) BOOL maximizeResolution;
@property (nonatomic) BOOL appleDNGWorkaround;
@property (nonatomic) BOOL bidirectionalCorrection;
@property (nonatomic) NSInteger antiAliasStrength;
@property (nonatomic) NSInteger luminance;
@property (nonatomic) NSInteger chroma;

@end
