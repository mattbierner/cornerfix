#include <CoreServices/CoreServices.h>

#include "FSArguments.h"
#include "CornerFixManager.h"
#include "ImageProcessor.h"

int main(int argc, const char * argv[])
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



    CornerFixManager* manager = [[CornerFixManager alloc] init];
        
    NSArray* inputs = [package allObjectsForSignature:inputFile];
    NSString* output = [package firstObjectForSignature:outputFile];

    if ([package booleanValueForSignature:createProfile]) {
        if ([inputs count] > 1) {
            BOOL isDir;
            if (![[NSFileManager defaultManager] fileExistsAtPath:output isDirectory:&isDir] || !isDir) {
                exit(EXIT_FAILURE);
            }
            
            [manager createProfiles:inputs ok:^{} err:^{}];
        } else {
            [manager createProfiles:inputs ok:^{} err:^{}];
        }
        
        exit(EXIT_SUCCESS);
    } else { // convert images
        NSString* profileFile = [package firstObjectForSignature:profile];
        if (!profileFile || ![[NSFileManager defaultManager] fileExistsAtPath:profileFile]) {
            exit(EXIT_FAILURE);
        }
        ImageProcessor* processor = [ImageProcessor processorForProfile:profileFile];

         if ([inputs count] > 1) {
            BOOL isDir;
            if (![[NSFileManager defaultManager] fileExistsAtPath:output isDirectory:&isDir] || !isDir) {
                exit(EXIT_FAILURE);
            }
        } else {
            [processor processFiles:inputs ok:^{} err:^{}];
        }
    }

    exit(EXIT_SUCCESS);

}
