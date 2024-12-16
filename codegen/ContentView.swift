import SwiftUI

struct CustomButtonStyle: ButtonStyle {
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding()
            .background(configuration.isPressed ? Color.gray : Color.blue)
            .foregroundStyle(.white)
            .cornerRadius(10)
            .frame(maxWidth: 200) // Ограничение ширины
            .frame(height: 60)
    }
}

struct ContentView: View {
    @State private var userInput: String = ""
    @State private var resultText: String = NSLocalizedString("Enter your query and click the button.", comment: "")


    
    var body: some View {
        VStack(alignment: .leading, spacing: 20) {
            Text("GPT Code Generation")
                .font(.headline)

            TextField(NSLocalizedString("Enter your request...", comment: ""), text: $userInput)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .padding(.bottom, 10)


            
            HStack() {
                Button(action: {
                    let output = CodegenWrapper.runCodegen(withInput: userInput)
                    self.resultText = output ?? "error"
                }) {
                    Text(NSLocalizedString("Generate code", comment: ""))
                }
                .buttonStyle(CustomButtonStyle())
                
                
                Button(action: {
//                    код для кнопки, еще не придумал
                }) {
                    Text(NSLocalizedString("Run program", comment: ""))
                }
                .buttonStyle(CustomButtonStyle())
            }
            .frame(maxWidth: .infinity, alignment: .center)
               
            
            Text(NSLocalizedString("Result:", comment: ""))
                .font(.subheadline)
            ScrollView {
                Text(resultText)
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding()
                    .background(Color.secondary.opacity(0.1))
                    .cornerRadius(8)
            }
        }
        .padding()
    }
}



#Preview {
    ContentView()
}
