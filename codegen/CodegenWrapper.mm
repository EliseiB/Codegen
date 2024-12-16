//
//  CodegenWrapper.mm
//  codegen
//
//  Created by Elisei Bigun on 12/14/24.
//

// CodegenWrapper.mm
#import "CodegenWrapper.h"
#include "codegen.hpp" // заголовок с run_codegen

@implementation CodegenWrapper

+ (NSString *)runCodegenWithInput:(NSString *)input {
    std::string cpp_input = [input UTF8String];
    std::string cpp_output = run_codegen(cpp_input);
    return [NSString stringWithUTF8String:cpp_output.c_str()];
}

@end
