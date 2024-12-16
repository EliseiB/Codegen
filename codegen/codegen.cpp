// codegen.cpp
// A program that utilizes the OpenAI GPT API to generate code based on user requests.

/**
 Run Program:

mkdir -p build
cd build
cmake ..
make

./codegen
**/

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

// For HTTP requests
#include <curl/curl.h>
// For JSON parsing
#include </Users/eliseibigun/Documents/Swift/codegen/json/include/nlohmann/json.hpp>  // Include nlohmann's JSON library

// For filesystem operations
#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;




#include "codegen.hpp"

// Подключаем все, что вам нужно,
// здесь будут все ваши include из вашего codegen.cpp
#include <iostream>
#include <streambuf>
#include <sstream>
#include <string>

// Предполагается, что в codegen.cpp у вас уже есть main2() и вся логика.
// Ниже мы только добавим run_codegen.

// Пример main2():
// int main2() {
//     // Здесь ваша логика работы с GPT, генерация кода, вывод результатов в std::cout
//     // Допустим, он читает что-то из std::cin, обращается к GPT и выводит результат
//     // ...
//     return 0;
// }

// Объявляем main2() чтобы использовать его внутри run_codegen
int main2();

// Теперь реализуем run_codegen
std::string run_codegen(const std::string &user_request) {
    // Сохраняем старый буфер ввода/вывода
    std::streambuf* old_cin_buf = std::cin.rdbuf();
    std::streambuf* old_cout_buf = std::cout.rdbuf();

    // Создаем stringstream для имитации ввода пользователя
    std::istringstream input_stream(user_request + "\n");
    std::cin.rdbuf(input_stream.rdbuf());

    // Создаем stringstream для перехвата вывода
    std::ostringstream output_stream;
    std::cout.rdbuf(output_stream.rdbuf());

    // Запускаем вашу основную логику
    main2();

    // Восстанавливаем стандартные буферы
    std::cin.rdbuf(old_cin_buf);
    std::cout.rdbuf(old_cout_buf);

    // Возвращаем то, что вывел main2()
    return output_stream.str();
}











/**
 * Callback function for libcurl to write received data into a string.
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * Parses the assistant's response to extract code blocks and their filenames.
 * Only processes code blocks labeled as C++ (```cpp).
 */
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
                // Start of code block
                in_code_block = true;
                code.clear();
                // Extract language and filename if present
                std::string lang_and_filename = line.substr(3);  // Skip ```
                size_t start = lang_and_filename.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    lang_and_filename = lang_and_filename.substr(start);
                }
                // Split language and filename
                std::istringstream iss(lang_and_filename);
                iss >> language >> filename;
                if (filename.empty()) {
                    filename = "code." + language;  // e.g., code.cpp
                }
            } else {
                // End of code block
                in_code_block = false;
                // Only save the code if it's in C++
                if (language == "cpp" || language == "c++") {
                    files.push_back({filename, code});
                } else {
                    std::cerr << "Skipping code block in language: " << language << std::endl;
                }
            }
        } else if (in_code_block) {
            code += line + "\n";
        }
    }
    return files;
}

/**
 * Generates a CMakeLists.txt file to build the project.
 */
void generate_cmake_lists(const std::vector<std::pair<std::string, std::string>>& files,
                          const std::vector<std::string>& libraries,
                          const std::vector<std::string>& include_dirs) {
    std::ofstream ofs("CodeGenerator/CMakeLists.txt");
    if (!ofs) {
        std::cerr << "Error writing CMakeLists.txt" << std::endl;
        return;
    }
    ofs << "cmake_minimum_required(VERSION 3.10)\n";
    ofs << "project(CodeGeneratorProject)\n\n";
    ofs << "set(CMAKE_CXX_STANDARD 17)\n\n";

    // Include directories
    for (const auto& dir : include_dirs) {
        ofs << "include_directories(" << dir << ")\n";
    }

    // Add executable
    ofs << "add_executable(CodeGeneratorProject";
    for (const auto& file : files) {
        ofs << " " << file.first;
    }
    ofs << ")\n\n";

    // Link libraries
    if (!libraries.empty()) {
        ofs << "target_link_libraries(CodeGeneratorProject";
        for (const auto& lib : libraries) {
            ofs << " " << lib;
        }
        ofs << ")\n";
    }
    ofs.close();
}

/**
 * Parses compiler error output to identify missing headers.
 */
std::vector<std::string> parse_missing_headers(const std::string& compiler_output) {
    std::vector<std::string> missing_headers;
    std::regex header_regex(R"(fatal error: ['<]([^'>]+)[">] file not found)");
    std::smatch match;
    std::istringstream ss(compiler_output);
    std::string line;
    while (std::getline(ss, line)) {
        if (std::regex_search(line, match, header_regex)) {
            missing_headers.push_back(match[1]);
        }
    }
    return missing_headers;
}

/**
 * Parses compiler error output to identify missing libraries.
 */
std::vector<std::string> parse_linker_errors(const std::string& compiler_output) {
    std::vector<std::string> missing_libraries;
    // Implement logic to parse linker errors and identify missing libraries
    // This can be complex and may require pattern matching specific to your environment
    return missing_libraries;
}

/**
 * Attempts to install missing dependencies using Homebrew.
 */
void install_dependencies(const std::vector<std::string>& headers) {
    // Map headers to Homebrew packages
    std::map<std::string, std::string> header_to_package = {
        {"cpprest/http_client.h", "cpprestsdk"},
        {"cpprest/filestream.h", "cpprestsdk"},
        {"cpprest/json.h", "cpprestsdk"},
        {"boost/multiprecision/cpp_int.hpp", "boost"},
        {"boost/algorithm/string.hpp", "boost"},
        {"SDL2/SDL.h", "sdl2"},
        {"SFML/Graphics.hpp", "sfml"},
        {"openssl/ssl.h", "openssl"},
        {"curses.h", "ncurses"},
        {"ncurses.h", "ncurses"},
        // Add more mappings as needed
    };

    for (const auto& header : headers) {
        auto it = header_to_package.find(header);
        if (it != header_to_package.end()) {
            std::string package = it->second;
            std::cout << "Installing package: " << package << " for header: " << header << std::endl;
            std::string command = "brew install " + package;
            int ret = std::system(command.c_str());
            if (ret != 0) {
                std::cerr << "Failed to install package: " << package << std::endl;
            }
        } else {
            std::cerr << "No known package for header: " << header << std::endl;
        }
    }
}

/**
 * Attempts to fix common syntax errors in the code.
 */
void fix_syntax_errors(const std::string& filepath) {
    // Read the code from the file
    std::ifstream ifs(filepath);
    if (!ifs) {
        std::cerr << "Error reading file: " << filepath << std::endl;
        return;
    }
    std::string code((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    bool modified = false;

    // Fix missing semicolons (simple heuristic)
    std::regex missing_semicolon_regex(R"(([^;\s])\n)");
    code = std::regex_replace(code, missing_semicolon_regex, "$1;\n");
    modified = true;

    // Fix other common syntax errors as needed

    if (modified) {
        // Write the modified code back to the file
        std::ofstream ofs(filepath);
        if (!ofs) {
            std::cerr << "Error writing file: " << filepath << std::endl;
            return;
        }
        ofs << code;
        ofs.close();
    }
}

int main2() {
    // Get user input
    std::string user_prompt;
    std::cout << "Please enter your request: ";
    std::getline(std::cin, user_prompt);

    // Get API key from environment variable
    const char* api_key = std::getenv("OPENAI_API_KEY");
//    const char* api_key = "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";
    if (!api_key) {
        std::cerr << "Error: OPENAI_API_KEY environment variable not set." << std::endl;
        return 1;
    }

    // Initialize libcurl
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        std::string readBuffer;

        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        
        // Set headers
        struct curl_slist* headers = NULL;
        std::string auth_header = std::string("Authorization: Bearer ") + api_key;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set POST data
        json json_data;
        json_data["model"] = "gpt-4o-mini";  // Use a valid model
        json_data["messages"] = {
            { {"role", "system"}, {"content",
                "You are a coding assistant specialized in C++ for macOS. When providing code, ensure that it is free of syntax errors and compiles successfully. \
Include any necessary library dependencies and instructions for compilation. \
Ensure the code is compatible with macOS and does not use Windows-specific libraries like conio.h. \
Prefer using standard libraries or widely supported libraries. \
If external libraries are required, ensure they are easily installable via Homebrew."
            } },
            { {"role", "user"}, {"content", user_prompt} }
        };
        std::string post_fields = json_data.dump();

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());

        // Set write callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse response
            json response_json = json::parse(readBuffer);

            // Check for API errors
            if (response_json.contains("error")) {
                std::cerr << "API Error: " << response_json["error"]["message"] << std::endl;
                return 1;
            }

            std::string assistant_content = response_json["choices"][0]["message"]["content"];

            // Debug output to see assistant's response
            std::cout << "Assistant's response:\n" << assistant_content << std::endl;

            // Parse code blocks from assistant's response
            auto files = parse_code_blocks(assistant_content);

            if (files.empty()) {
                std::cerr << "No C++ code blocks found in the assistant's response." << std::endl;
                return 1;
            }

            // Create CodeGenerator directory
            fs::create_directory("CodeGenerator");

            // Write files to CodeGenerator directory
            for (const auto& file : files) {
                std::string filepath = "CodeGenerator/" + file.first;
                std::ofstream ofs(filepath);
                if (ofs) {
                    ofs << file.second;
                    ofs.close();
                } else {
                    std::cerr << "Error writing to file " << filepath << std::endl;
                }
            }

            // Initialize required libraries and include directories
            std::vector<std::string> libraries_needed;
            std::vector<std::string> include_dirs;

            // Determine required libraries
            for (const auto& file : files) {
                const std::string& code = file.second;

                // Check for known headers and map to libraries
                if (code.find("#include <ncurses.h>") != std::string::npos ||
                    code.find("#include <curses.h>") != std::string::npos) {
                    libraries_needed.push_back("ncurses");
                }
                if (code.find("#include <cpprest/http_client.h>") != std::string::npos) {
                    libraries_needed.push_back("cpprestsdk");
                }
                if (code.find("#include <boost/") != std::string::npos) {
                    libraries_needed.push_back("boost");
                }
                if (code.find("#include <openssl/") != std::string::npos) {
                    libraries_needed.push_back("ssl");
                    libraries_needed.push_back("crypto");
                }
                // Add more checks as needed
            }

            // Remove duplicate libraries
            std::sort(libraries_needed.begin(), libraries_needed.end());
            libraries_needed.erase(std::unique(libraries_needed.begin(), libraries_needed.end()), libraries_needed.end());

            // Generate CMakeLists.txt with the required libraries
            generate_cmake_lists(files, libraries_needed, include_dirs);

            bool success = false;
            int attempt = 0;
            const int max_attempts = 3;

            while (!success && attempt < max_attempts) {
                attempt++;

                // Change current directory to CodeGenerator
                fs::current_path("CodeGenerator");

                // Run cmake
                int ret = std::system("cmake .");
                if (ret != 0) {
                    std::cerr << "Error running cmake." << std::endl;
                    return 1;
                }

                // Run make and capture compiler output
                std::string compiler_output;
                FILE* pipe = popen("make 2>&1", "r");
                if (!pipe) {
                    std::cerr << "Error running make." << std::endl;
                    return 1;
                }
                char buffer[128];
                while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                    compiler_output += buffer;
                    std::cout << buffer;  // Output compiler messages
                }
                int status = pclose(pipe);
                if (status == 0) {
                    success = true;
                    break;
                } else {
                    std::cerr << "Compilation failed. Attempting to detect and install missing dependencies..." << std::endl;

                    // Parse missing headers
                    auto missing_headers = parse_missing_headers(compiler_output);
                    if (!missing_headers.empty()) {
                        install_dependencies(missing_headers);
                    }

                    // Attempt to fix syntax errors
                    for (const auto& file : files) {
                        std::string filepath = file.first;
                        fix_syntax_errors(filepath);
                    }

                    // Update required libraries based on new dependencies
                    // (Re-extract required libraries and regenerate CMakeLists.txt)
                    // This can be repeated or refined as needed
                }

                // Change back to the parent directory
                fs::current_path("..");
            }

            if (!success) {
                std::cerr << "Compilation failed after multiple attempts." << std::endl;
                return 1;
            }

            // Run the compiled program
            int ret = std::system("./CodeGeneratorProject");
            if (ret != 0) {
                std::cerr << "Error running the program." << std::endl;
                return 1;
            }
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        std::cerr << "Error initializing curl." << std::endl;
        return 1;
    }

    return 0;
}
