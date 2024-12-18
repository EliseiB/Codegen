import SwiftUI
import Cocoa

struct ContentView: View {
    @State private var input: String = "" // Поле ввода текста
    @State private var result: String = NSLocalizedString("Enter your input", comment: "Initial result text")
    @State private var sourcePath: URL? // Путь к исходному коду
    @State private var outputPath: URL? // Путь к исполняемому файлу
    @State private var btnIsActive = false
    @State private var selectedModel: String = "GPT-4o" // Выбранная модель по умолчанию
    @State private var userApiKey: String = ""

    let models = [
        "GPT-4o": NSLocalizedString("Great for most tasks", comment: "Model description"),
        "01": NSLocalizedString("Uses advanced reasoning", comment: "Model description"),
        "01-mini": NSLocalizedString("Faster at reasoning", comment: "Model description")
    ]
    let moreModels = [
        NSLocalizedString("More models", comment: "Menu option"),
        NSLocalizedString("Temporary chat", comment: "Menu option")
    ]

    var isButtonDisabled: Bool {
        sourcePath == nil // Кнопка отключена, если sourcePath nil
    }
    
    var body: some View {
        
        VStack(spacing: 20) {
            
            HStack(){
                // Меню выбора модели (как на вашем скриншоте)
                VStack(alignment: .leading, spacing: 10) {
                    Text(NSLocalizedString("Model", comment: "Model label"))
                        .font(.headline)
                        .foregroundColor(.gray)
                    
                    Menu {
                        Section(header: Text(NSLocalizedString("Main Models", comment: "Section header"))) {
                            ForEach(models.keys.sorted(), id: \.self) { model in
                                Button(action: {
                                    selectedModel = model
                                }) {
                                    HStack {
                                        Text(model)
                                        Spacer()
                                        if selectedModel == model {
                                            Image(systemName: "checkmark")
                                        }
                                    }
                                }
                            }
                        }
                        Section(header: Text(NSLocalizedString("Other Options", comment: "Section header"))) {
                            ForEach(moreModels, id: \.self) { option in
                                Button(action: {
                                    print("\(option) selected")
                                }) {
                                    Text(option)
                                }
                            }
                        }
                    } label: {
                        HStack {
                            VStack(alignment: .leading) {
                                Text(selectedModel)
                                    .font(.title3)
                                    .foregroundColor(.white)
                                Text(models[selectedModel] ?? "")
                                    .font(.caption)
                                    .foregroundColor(.gray)
                            }
                            Spacer()
                            Image(systemName: "chevron.down")
                                .foregroundColor(.gray)
                        }
                        .padding(.horizontal, 8)
                        .frame(width: 150, height: 28) // Ограничиваем ширину меню
                        .background(Color.gray.opacity(0.3))
                        .cornerRadius(6)
                    }
                    .frame(width: 150)
                }
                .padding(.horizontal, 8)
                
                
                
                Group() {
                    
                    TextField(NSLocalizedString("Enter your API key (optional)", comment: "API Key placeholder"), text: $userApiKey)
                        .foregroundColor(.white)
                        .padding(10)
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(Color.gray.opacity(0.2))
                        .cornerRadius(10)
                }
                
                
            }
            
            // Поле результата
            ScrollView {
                Text(result)
                    .padding()
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .background(Color.gray.opacity(0.2))
                    .cornerRadius(20)
            }
            .padding(.horizontal)
            
            // Кнопка Build and Run
            Button(NSLocalizedString("Build and Run", comment: "Button title")) {
                print("Build and Run button pressed")
                guard let source = sourcePath else {
                    self.result = NSLocalizedString("Source file not found. Please generate the code first.", comment: "Error message when source file is missing")
                    return
                }
                DispatchQueue.global(qos: .background).async {
                    let executableName = "generated_executable"
                    let executableURL = source.deletingLastPathComponent().appendingPathComponent(executableName)
                    self.outputPath = executableURL
                    
                    let buildResult = CodegenWrapper.buildAndRunApplication(withSourcePath: source.path, outputExecutablePath: executableURL.path)
                    print("Build Result: \(buildResult)")
                    DispatchQueue.main.async {
                        self.result = buildResult
                    }
                }
            }
            .buttonStyle(CustomButtonStyle(isDisabled: isButtonDisabled))
            .disabled(isButtonDisabled)
        }
        .padding()
        .background(Color.black.edgesIgnoringSafeArea(.all))
        
        Spacer()
        ZStack(alignment: .bottomTrailing) {
            RoundedRectangle(cornerRadius: 20)
                .fill(Color.gray.opacity(0.2))
                .frame(height: 50)
            
            HStack {
                // Левые иконки
                HStack(spacing: 12) {
                    Image(systemName: "paperclip")
                    Image(systemName: "gift")
                    Image(systemName: "globe")
                }
                .foregroundColor(.gray)
                .padding(.leading, 12)
                
                // Поле для ввода
                TextField(NSLocalizedString("Message ChatGPT", comment: "Chat input placeholder"), text: $input)
                    .foregroundColor(.white)
                    .padding(8)
                    .textFieldStyle(PlainTextFieldStyle())
            }.padding(.bottom, 8)
            
            
//            Spacer()
            
            // Кнопка отправки
//            Button(action: {
//                btnIsActive = true
//                print("Generate Code button pressed")
//                
//                DispatchQueue.global(qos: .background).async {
////                    let generatedCode = CodegenWrapper.generateCode(withInput: input)
//                    let generatedCode = CodegenWrapper.generateCode(withInput: input, apiKey: userApiKey.isEmpty ? nil : userApiKey)
//                    print("Generated Code: \(generatedCode)")
//                    
//                    DispatchQueue.main.async {
//                        self.result = generatedCode
//                        if let tempDir = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first {
//                            let cppFileURL = tempDir.appendingPathComponent("generated_code.cpp")
//                            do {
//                                try generatedCode.write(to: cppFileURL, atomically: true, encoding: .utf8)
//                                self.sourcePath = cppFileURL
//                                print("Code saved to \(cppFileURL.path)")
//                            } catch {
//                                self.result = String(format: NSLocalizedString("Error saving code: %@", comment: "Error message when saving fails"), error.localizedDescription)
//                            }
//                        }
//                        btnIsActive = false
//                    }
//                }
//            }) {
//                Image(systemName: input.isEmpty || btnIsActive ? "xmark" : "arrow.up.circle.fill")
//                    .foregroundColor(.white)
//                    .font(.system(size: 24))
//                    .padding(8)
//            }
//            .disabled(btnIsActive || input.isEmpty)
//            //.padding(16)
//            //.padding(.bottom, 8)
//            .background(input.isEmpty || btnIsActive ? Color.gray : Color.blue)
//            .clipShape(Circle())
////            .frame(alignment: .bottomTrailing)
            
            Button(action: {
                btnIsActive = true
                print("Generate Code button pressed")
                
                DispatchQueue.global(qos: .background).async {
                    let generatedCode = CodegenWrapper.generateCode(withInput: input, apiKey: userApiKey.isEmpty ? nil : userApiKey)
                    print("Generated Code: \(generatedCode)")
                    
                    DispatchQueue.main.async {
                        self.result = generatedCode
                        
                        // Проверка, что код сгенерирован корректно
                        if generatedCode.isEmpty || generatedCode.starts(with: "Ошибка") {
                            self.result = "Ошибка: код не сгенерирован или произошла ошибка API."
                            btnIsActive = false
                            return
                        }

                        // Запуск диалога для сохранения файла
                        if let saveURL = saveFileDialog(defaultName: "generated_code.cpp") {
                            do {
                                try generatedCode.write(to: saveURL, atomically: true, encoding: .utf8)
                                self.sourcePath = saveURL
                                print("Code saved to \(saveURL.path)")
                            } catch {
                                self.result = "Ошибка при сохранении файла: \(error.localizedDescription)"
                            }
                        } else {
                            self.result = "Сохранение отменено."
                        }
                        btnIsActive = false
                    }

                }
            }) {
                Image(systemName: input.isEmpty || btnIsActive ? "xmark" : "arrow.up.circle.fill")
                    .foregroundColor(.white)
                    .font(.system(size: 24))
                    .padding(8)
            }
            .disabled(btnIsActive || input.isEmpty)
            .background(input.isEmpty || btnIsActive ? Color.gray : Color.blue)
            .clipShape(Circle())

            
            
        }
        .padding(.horizontal)
//        .padding(.trailing, 40)
//        padding(.bottom, 8) errror
        Spacer(minLength: 10)
        
    }
}


func saveFileDialog(defaultName: String) -> URL? {
    let savePanel = NSSavePanel()
    savePanel.title = "Save Generated Code"
    savePanel.nameFieldStringValue = defaultName
    savePanel.allowedFileTypes = ["cpp"]
    
    if savePanel.runModal() == .OK {
        return savePanel.url
    }
    return nil
}


struct CustomButtonStyle: ButtonStyle {
    var isDisabled: Bool
    
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding()
            .background(isDisabled ? Color.gray : Color.blue)
            .foregroundColor(.white)
            .cornerRadius(8)
            .opacity(isDisabled ? 0.6 : 1.0)
            .scaleEffect(configuration.isPressed ? 0.95 : 1.0)
    }
}
