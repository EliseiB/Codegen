// codegen.cpp
#include "codegen.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <sstream>
#include <regex>
#include <map>
#include <algorithm>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;

// Callback-функция для записи ответа от сервера в строку
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функция для извлечения блоков кода из ответа
std::vector<std::pair<std::string, std::string>> parse_code_blocks(const std::string& content) {
    std::vector<std::pair<std::string, std::string>> files;
    std::istringstream ss(content);
    std::string line;
    bool in_code_block = false;
    std::string filename = "code.cpp";  // Default filename
    std::string code;
    std::string language;

    while (std::getline(ss, line)) {
        if (line.substr(0, 3) == "```") {
            if (!in_code_block) {
                // Начало блока кода
                in_code_block = true;
                code.clear();
                // Извлекаем язык и имя файла, если указаны
                std::string lang_and_filename = line.substr(3);  // Пропустить ```
                size_t start = lang_and_filename.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    lang_and_filename = lang_and_filename.substr(start);
                }
                std::istringstream iss(lang_and_filename);
                iss >> language >> filename;
                if (filename.empty()) {
                    filename = "code." + language;  // e.g., code.cpp
                }
            } else {
                // Конец блока кода
                in_code_block = false;
                // Сохраняем только блоки с C++
                if (language == "cpp" || language == "c++") {
                    files.push_back({filename, code});
                }
            }
        } else if (in_code_block) {
            code += line + "\n";
        }
    }
    return files;
}

const char* generateCode(const char* input, const char* userApiKey) {
    static std::string result;

    const char* api_key = userApiKey != nullptr ? userApiKey : std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        result = "Ошибка: API-ключ не предоставлен и не установлен в окружении.";
        std::cerr << result << std::endl;
        return result.c_str();
    }
    std::cout << "Используемый API-ключ: " << api_key << std::endl;

//const char* generateCode(const char* input) {
//    static std::string result;
//
//    // Проверка API-ключа
//    const char* api_key = std::getenv("OPENAI_API_KEY");
//    if (!api_key) {
//        result = "Ошибка: переменная окружения OPENAI_API_KEY не установлена.";
//        std::cerr << result << std::endl;
//        return result.c_str();
//    }
//    std::cout << "API-ключ загружен: " << api_key << std::endl;

    // Инициализация curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        result = "Ошибка: не удалось инициализировать libcurl.";
        std::cerr << result << std::endl;
        return result.c_str();
    }

    // Подготовка JSON-запроса
    json request_data = {
        {"model", "gpt-4o"},
        {"messages", {
            {{"role", "system"}, {"content", "You are a coding assistant specialized in C++ for macOS. When providing code, ensure that it is free of syntax errors and compiles successfully. \
Include any necessary library dependencies and instructions for compilation. \
Ensure the code is compatible with macOS and does not use Windows-specific libraries like conio.h. \
Prefer using standard libraries or widely supported libraries. \
If external libraries are required, ensure they are easily installable via Homebrew."}},
            {{"role", "user"}, {"content", input}}
        }},
        {"temperature", 0.3},
        {"max_tokens", 4000}
    };

    std::string request_body = request_data.dump();
    std::cout << "Отправляемый JSON:\n" << request_body << std::endl;

    // Настройка заголовков
    std::string readBuffer;
    struct curl_slist* headers = NULL;
    std::string auth_header = "Authorization: Bearer " + std::string(api_key);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());

    // Настройка curl
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // Выполнение запроса
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        result = "Ошибка при выполнении запроса: " + std::string(curl_easy_strerror(res));
        std::cerr << result << std::endl;
    } else {
        std::cout << "Ответ сервера OpenAI:\n" << readBuffer << std::endl;

        // Обработка JSON-ответа
        try {
            auto json_response = json::parse(readBuffer);

            if (json_response.contains("choices") && !json_response["choices"].empty()) {
                std::string content = json_response["choices"][0]["message"]["content"].get<std::string>();

                // Извлечение только кода из ответа
                auto files = parse_code_blocks(content);
                if (!files.empty()) {
                    result = files[0].second;  // Сохраняем первый блок кода
                } else {
                    result = "Ошибка: код не найден в ответе.";
                }
            } else {
                result = "Ответ не содержит ожидаемых данных.";
            }
        } catch (const std::exception& e) {
            result = "Ошибка парсинга JSON: ";
            result += e.what();
        }
    }

    // Очистка ресурсов
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return result.c_str();
}

//const char* buildAndRunApplication(const char* sourceCodePath, const char* outputExecutablePath) {
//    static std::string result;
//
//    // Компиляция программы
//    std::ostringstream compileCommand;
//    compileCommand << "g++ " << "\"" << sourceCodePath << "\"" << " -o " << "\"" << outputExecutablePath << "\"";
//    std::cout << "Компиляция программы: " << compileCommand.str() << std::endl;
//
//    int compileStatus = std::system(compileCommand.str().c_str());
//    if (compileStatus != 0) {
//        result = "Ошибка компиляции.";
//        return result.c_str();
//    }
//
//    // Запуск программы и захват вывода
//    std::string runCommand = "\"" + std::string(outputExecutablePath) + "\"";
//    std::array<char, 128> buffer;
//    std::string commandOutput;
//    FILE* pipe = popen(runCommand.c_str(), "r");
//    if (!pipe) {
//        result = "Не удалось запустить приложение.";
//        return result.c_str();
//    }
//
//    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
//        commandOutput += buffer.data();
//    }
//    int returnCode = pclose(pipe);
//
//    if (returnCode != 0) {
//        result = "Приложение завершилось с ошибкой:\n" + commandOutput;
//        return result.c_str();
//    }
//
//    result = "Приложение успешно запущено. Вывод:\n" + commandOutput;
//    return result.c_str();
//}

//const char* buildAndRunApplication(const char* sourceCodePath, const char* outputExecutablePath) {
//    static std::string result;
//
//    // Компиляция программы
//    std::ostringstream compileCommand;
//    compileCommand << "g++ " << "\"" << sourceCodePath << "\"" << " -o " << "\"" << outputExecutablePath << "\"";
//    int compileStatus = std::system(compileCommand.str().c_str());
//    if (compileStatus != 0) {
//        result = "Ошибка компиляции.";
//        return result.c_str();
//    }
//
//    // Запуск программы в новой вкладке терминала
//    std::ostringstream runCommand;
//    runCommand << "osascript -e 'tell application \"Terminal\" to do script \""
//               << outputExecutablePath << "\"'";
//
//    int runStatus = std::system(runCommand.str().c_str());
//    if (runStatus != 0) {
//        result = "Не удалось открыть программу в терминале.";
//        return result.c_str();
//    }
//
//    result = "Программа успешно запущена в новой вкладке терминала!";
//    return result.c_str();
//}


const char* buildAndRunApplication(const char* sourceCodePath, const char* outputExecutablePath) {
    static std::string result;

    // Компиляция программы с флагом стандарта C++11
    std::ostringstream compileCommand;
    compileCommand << "g++ -std=c++11 " << "\"" << sourceCodePath << "\"" << " -o " << "\"" << outputExecutablePath << "\"";
    int compileStatus = std::system(compileCommand.str().c_str());

    if (compileStatus != 0) {
        result = "Ошибка компиляции.";
        return result.c_str();
    }

    // Запуск программы в новой вкладке терминала
    std::ostringstream runCommand;
    runCommand << "osascript -e 'tell application \"Terminal\" to do script \""
               << outputExecutablePath << "\"'";

    int runStatus = std::system(runCommand.str().c_str());
    if (runStatus != 0) {
        result = "Не удалось открыть программу в терминале.";
        return result.c_str();
    }

    result = "Программа успешно запущена в новой вкладке терминала!";
    return result.c_str();
}
