/**
 * CornerFix command line utility.
 */
#include <CoreServices/CoreServices.h>

#include "FSArguments.h"

#include "CornerFixManager.h"
#include "ImageProcessor.h"


void createProfiles(NSArray* inputs, NSString* output) {
    if ([inputs count] > 1) {
        BOOL isDir;
        if (![[NSFileManager defaultManager] fileExistsAtPath:output isDirectory:&isDir] || !isDir) {
            exit(EXIT_FAILURE);
        }
    }
    
    CornerFixManager* manager = [[CornerFixManager alloc] init];
    [manager createProfiles:inputs ok:^{} err:^{}];
    [manager release];
    
    exit(EXIT_SUCCESS);
}

void convertImages(NSString* profileFile, NSArray* inputs, NSString* output) {
    if (!profileFile || ![[NSFileManager defaultManager] fileExistsAtPath:profileFile]) {
        exit(EXIT_FAILURE);
    }
    if ([inputs count] > 1) {
        BOOL isDir;
        if (![[NSFileManager defaultManager] fileExistsAtPath:output isDirectory:&isDir] || !isDir) {
            exit(EXIT_FAILURE);
        }
    }
    
    ImageProcessor* processor = [ImageProcessor processorForProfile:profileFile config:[NSUserDefaults standardUserDefaults]];
    [processor processFiles:inputs ok:^{} err:^{}];
}

int main(int __unused argc, const char* __unused argv[])
{
    FSArgumentSignature
        * help = [FSArgumentSignature argumentSignatureWithFormat:@"[-h --help]"],
        * createProfile = [FSArgumentSignature argumentSignatureWithFormat:@"[-m --make-profile]"],
    
        * profile = [FSArgumentSignature argumentSignatureWithFormat:@"[-p --profile]="],
        * config = [FSArgumentSignature argumentSignatureWithFormat:@"[-c --config]="],
    
        * outputFile = [FSArgumentSignature argumentSignatureWithFormat:@"[-o --output of]="],
    
        * inputFile = [FSArgumentSignature argumentSignatureWithFormat:@"[-i --input if]={1,}"];

    NSArray* signatures = @[help, createProfile, profile, config, outputFile, inputFile];

    FSArgumentPackage* package = [[NSProcessInfo processInfo] fsargs_parseArgumentsWithSignatures:signatures];
    
    NSArray* inputs = [package allObjectsForSignature:inputFile];
    NSString* output = [package firstObjectForSignature:outputFile];

    if ([package booleanValueForSignature:createProfile]) {
        createProfiles(inputs, output);
    } else { // convert images
        NSString* profileFile = [package firstObjectForSignature:profile];
        convertImages(profileFile, inputs, output);
    }

    exit(EXIT_SUCCESS);

}
