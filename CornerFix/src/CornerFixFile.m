//
//  CornerFixFile.m
//  CornerFix
//
//  Created by Matt Bierner on 5/21/14.
//  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
//

#import "CornerFixFile.h"

@implementation CornerFixFile

+ (NSString *) convertExtension:(NSString*) file extension: (NSString*) extension
{
    return [[file stringByDeletingPathExtension] stringByAppendingPathExtension:extension];
}


+ (NSArray *) convertExtensions:(NSArray *) files extension: (NSString *) extension
{
	if (files && extension) 
	{
		NSMutableArray *newArray = [NSMutableArray arrayWithCapacity:[files count]];
		for (NSString* file in files)
			[newArray addObject:[CornerFixFile convertExtension:file extension:extension]];
		return newArray;
	}
    return nil;
}


+ (NSString *) renameFile:(NSString*) file suffix:(NSString*) suffix
{
    return [[[file stringByDeletingPathExtension] stringByAppendingString:suffix] stringByAppendingPathExtension:[file pathExtension]];
}


+ (NSArray *) renameFiles:(NSArray *) files suffix: (NSString *) suffix
{
	if (files && suffix) 
	{
        NSMutableArray *newArray = [NSMutableArray arrayWithCapacity:[files count]];
		for (NSString* file in files)
			[newArray addObject:[CornerFixFile renameFile: file suffix:suffix]];
		return newArray;
	}
    return nil;
}


@end
