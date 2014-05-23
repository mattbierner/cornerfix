#import <Foundation/Foundation.h>

@interface CornerFixFile : NSObject

+ (NSString *) convertExtension:(NSString*) file extension: (NSString*) extension;

+ (NSArray *) convertExtensions:(NSArray*) files extension: (NSString*) extension;

+ (NSString *) renameFile:(NSString*) file suffix:(NSString*) suffix;

+ (NSArray *) renameFiles:(NSArray*) files suffix: (NSString*) suffix;

@end
