//
//  CornerFixFile.h
//  CornerFix
//
//  Created by Matt Bierner on 5/21/14.
//  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CornerFixFile : NSObject

+ (NSString *) convertExtension:(NSString*) file extension: (NSString*) extension;

+ (NSArray *) convertExtensions:(NSArray *) files extension: (NSString *) extension;

+ (NSString *) renameFile:(NSString*) file suffix:(NSString*) suffix;

+ (NSArray *) renameFiles:(NSArray *) files suffix: (NSString *) suffix;

@end
