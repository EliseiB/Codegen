// CodegenWrapper.mm
// Реализация интерфейса для взаимодействия с C++ кодом.

#import "CodegenWrapper.h"
#include "codegen.hpp"

@implementation CodegenWrapper

+ (NSString *)generateCodeWithInput:(NSString *)input {
    NSLog(@"generateCodeWithInput called with input: %@", input);
    const char *cInput = [input UTF8String];
    const char *cOutput = generateCode(cInput);

    if (cOutput != nullptr) {
        NSString *outputString = [NSString stringWithUTF8String:cOutput];
        NSLog(@"generateCodeWithInput output: %@", outputString);
        return outputString;
    } else {
        NSLog(@"generateCodeWithInput failed.");
        return @"Ошибка: не удалось сгенерировать код.";
    }
}

+ (NSString *)buildAndRunApplicationWithSourcePath:(NSString *)sourcePath
                           outputExecutablePath:(NSString *)outputPath {
    NSLog(@"buildAndRunApplicationWithSourcePath called with sourcePath: %@, outputPath: %@", sourcePath, outputPath);
    const char *cSourcePath = [sourcePath UTF8String];
    const char *cOutputPath = [outputPath UTF8String];
    const char *cResult = buildAndRunApplication(cSourcePath, cOutputPath);

    if (cResult != nullptr) {
        NSString *resultString = [NSString stringWithUTF8String:cResult];
        NSLog(@"buildAndRunApplicationWithSourcePath output: %@", resultString);
        return resultString;
    } else {
        NSLog(@"buildAndRunApplicationWithSourcePath failed.");
        return @"Ошибка: не удалось скомпилировать и запустить приложение.";
    }
}

+ (NSString *)generateCodeWithInput:(NSString *)input apiKey:(nullable NSString *)apiKey {
    NSLog(@"generateCodeWithInput called with input: %@", input);
    const char *cInput = [input UTF8String];
    const char *cApiKey = apiKey != nil ? [apiKey UTF8String] : nullptr;
    const char *cOutput = generateCode(cInput, cApiKey);

    if (cOutput != nullptr) {
        NSString *outputString = [NSString stringWithUTF8String:cOutput];
        NSLog(@"generateCodeWithInput output: %@", outputString);
        return outputString;
    } else {
        NSLog(@"generateCodeWithInput failed.");
        return @"Ошибка: не удалось сгенерировать код.";
    }
}


@end
