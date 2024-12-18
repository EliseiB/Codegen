//
//  codegen.hpp
//  codegen
//
//  Created by Elisei Bigun on 12/14/24.
//


// codegen.hpp
// codegen.hpp
#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <string>

// Функция для генерации кода на основе пользовательского ввода

// Функция для компиляции и запуска приложения
const char* buildAndRunApplication(const char* sourceCodePath, const char* outputExecutablePath);


const char* generateCode(const char* input, const char* apiKey = nullptr);
#endif // CODEGEN_HPP
