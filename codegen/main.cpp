// main.cpp
#include "codegen.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    // Получение API-ключа из переменной окружения
    const char* apiKeyEnv = std::getenv("OPENAI_API_KEY");
    if (!apiKeyEnv) {
        std::cerr << "Ошибка: Переменная окружения OPENAI_API_KEY не установлена." << std::endl;
        return 1;
    }
    std::string apiKey(apiKeyEnv);

    // Создание экземпляра CodeGenerator
    CodeGenerator generator(apiKey);

    // Получение запроса от пользователя
    std::string userPrompt;
    std::cout << "Пожалуйста, введите ваш запрос: ";
    std::getline(std::cin, userPrompt);

    // Запрос к OpenAI API
    std::string assistantResponse;
    if (!generator.fetchCodeFromAPI(userPrompt, assistantResponse)) {
        generator.log("Не удалось получить ответ от OpenAI API.");
        return 1;
    }

    // Вывод ответа ассистента
    std::cout << "Ответ ассистента:\n" << assistantResponse << std::endl;

    // Парсинг кодовых блоков
    auto codeFiles = generator.parseCodeBlocks(assistantResponse);
    if (codeFiles.empty()) {
        generator.log("Не найдены кодовые блоки на C++ в ответе ассистента.");
        return 1;
    }

    // Создание директории для генерации кода
    std::string projectDir = "CodeGenerator";
    if (!fs::exists(projectDir)) {
        fs::create_directory(projectDir);
        generator.log("Создана директория: " + projectDir);
    } else {
        generator.log("Директория уже существует: " + projectDir);
    }

    // Запись файлов кода
    for (const auto& [filename, code] : codeFiles) {
        fs::path filepath = fs::path(projectDir) / filename;
        std::ofstream ofs(filepath);
        if (!ofs.is_open()) {
            generator.log("Не удалось создать файл: " + filepath.string());
            continue;
        }
        ofs << code;
        ofs.close();
        generator.log("Создан файл: " + filepath.string());
    }

    // Определение необходимых библиотек и директорий заголовочных файлов
    std::vector<std::string> librariesNeeded;
    std::vector<std::string> includeDirs;

    for (const auto& [filename, code] : codeFiles) {
        if (code.find("#include <nlohmann/json.hpp>") != std::string::npos) {
            librariesNeeded.push_back("nlohmann_json::nlohmann_json");
            includeDirs.push_back("/usr/local/include"); // Путь по умолчанию для Homebrew
        }
        if (code.find("#include <curl/curl.h>") != std::string::npos) {
            librariesNeeded.push_back("curl");
            includeDirs.push_back("/usr/local/include"); // Путь по умолчанию для Homebrew
        }
        // Добавьте другие зависимости по необходимости
    }

    // Генерация CMakeLists.txt
    if (!generator.generateCMakeLists(projectDir, codeFiles, librariesNeeded, includeDirs)) {
        generator.log("Не удалось сгенерировать CMakeLists.txt.");
        return 1;
    }

    // Компиляция проекта
    std::string buildOutput;
    if (!generator.buildProject(projectDir, buildOutput)) {
        generator.log("Сборка проекта завершилась с ошибкой.");
        return 1;
    }

    // Определение пути к исполняемому файлу
    std::string executablePath = "CodeGeneratorProject";

    // Запуск приложения
    std::string runOutput;
    if (!generator.runApplication(executablePath, runOutput)) {
        generator.log("Запуск приложения завершился с ошибкой.");
        return 1;
    }

    return 0;
}
