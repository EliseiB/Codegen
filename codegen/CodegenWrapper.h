// CodegenWrapper.h
// Интерфейс для экспорта функций C++ в Swift.

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CodegenWrapper : NSObject

// Метод с внешней меткой
+ (NSString *)generateCodeWithInput:(NSString *)input;

// Метод с внешними метками для обоих параметров
+ (NSString *)buildAndRunApplicationWithSourcePath:(NSString *)sourcePath
                           outputExecutablePath:(NSString *)outputPath;

+ (NSString *)generateCodeWithInput:(NSString *)input apiKey:(nullable NSString *)apiKey;


@end

NS_ASSUME_NONNULL_END
