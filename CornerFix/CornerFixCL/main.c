/**
 * CornerFix command line utility.
 */
#include <CoreServices/CoreServices.h>

#include "FSArguments.h"

#include "CornerFixManager.h"
#include "CornerFixConfiguration.h"
#include "CornerFixVersion.h"
#include "ImageProcessor.h"
#include "CornerFixFile.h"

#include <iostream>


NSString* getVersion() {
    return [NSString stringWithUTF8String:CornerFixVersion];
}

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

bool convertImages(NSString* profileFile, NSArray* inputs, NSString* output) {
    if (!profileFile || ![[NSFileManager defaultManager] fileExistsAtPath:profileFile])
        return false;
    
    if ([inputs count] > 1) {
        BOOL isDir;
        if (![[NSFileManager defaultManager] fileExistsAtPath:output isDirectory:&isDir] || !isDir) {
            NSLog(@"Multiple inputs specified, expected existing output directory");
            return false;
        }
    }
    
    NSError* configError;
    ImageProcessor* processor = [ImageProcessor
        processorForProfile:profileFile
        config:[CornerFixConfiguration defaults]
        error:&configError];
    
    if (!processor) {
        NSLog(@"%@", [configError localizedDescription]);
        return false;
    }
    
    process_file_success ok = ^{};
    process_file_failure err = ^{};
    
    if ([inputs count] > 1) {
        [processor
            processFiles:inputs
            outputName:^NSString*(NSString* file) {
                NSURL* rel = [NSURL URLWithString:file relativeToURL:[NSURL URLWithString:output]];
                NSURL* url = [NSURL fileURLWithPathComponents:
                    rel.path.length == 0 ? @[output, file] : @[output, [rel path]]];
                return [CornerFixFile renameFile:url.relativePath suffix:@"_CF"];
            }
            ok:ok
            err:err];
    } else {
        [processor
            processFile:[inputs objectAtIndex:0]
            output:output
            ok:ok
            err:err];
    }
    
    return true;
}

int main(int __unused argc, const char* __unused argv[])
{
    FSArgumentSignature
        * help = [FSArgumentSignature argumentSignatureWithFormat:@"[-h --help]"],
        * version = [FSArgumentSignature argumentSignatureWithFormat:@"[--version]"],

        * createProfile = [FSArgumentSignature argumentSignatureWithFormat:@"[-m --make-profile]"],
    
        * profile = [FSArgumentSignature argumentSignatureWithFormat:@"[-p --profile]="],
        * config = [FSArgumentSignature argumentSignatureWithFormat:@"[-c --config]="],
    
        * outputFile = [FSArgumentSignature argumentSignatureWithFormat:@"[-o --output]="],
        * inputFile = [FSArgumentSignature argumentSignatureWithFormat:@"[-i --input]={1,}"];

    NSArray* signatures = @[help, version, createProfile, profile, config, outputFile, inputFile];

    FSArgumentPackage* package = [[NSProcessInfo processInfo] fsargs_parseArgumentsWithSignatures:signatures];

    
    if ([package booleanValueForSignature:help]) {
        std::cout
            << "Cornerfix " << [getVersion() UTF8String] << std::endl
            << std::endl
            << "\tcornerfix [--config=config.plist] --profile=lens.cpf -o corrected.dng -i input.dng ..." << std::endl
            << "\tcornerfix --make-profile [--config=config.plist] -o profile.cpf -i reference_image.dng ..." << std::endl
            << "\tcornerfix [-h --help] [--version]" << std::endl
            << std::endl
            << "\t-h --help\tShow this help message." << std::endl
            << "\t--version\tPrint version." << std::endl
            << "\t-m --make-profile\tCreate cornerfix lens profiles from input DNG files and write CPF to outputs." << std::endl
            << "\t-p --profile=file.cpf\tCPF lens correction profile." << std::endl
            << "\t-c --config=config.plist\tOptional CornerFix configuration." << std::endl;

        exit(EXIT_SUCCESS);
    }
    
    if ([package booleanValueForSignature:version]) {
        std::cout << [getVersion() UTF8String] << std::endl;
        exit(EXIT_SUCCESS);
    }
    
    
    NSArray* inputs = [package allObjectsForSignature:inputFile];
    NSString* output = [package firstObjectForSignature:outputFile];

    if ([package booleanValueForSignature:createProfile]) {
        createProfiles(inputs, output);
    } else { // convert images
        NSString* profileFile = [package firstObjectForSignature:profile];
        bool success = convertImages(profileFile, inputs, output);
        exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
