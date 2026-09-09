#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#endif

#include <process/BaseProcess.hpp>
#include <http/HTTPClient.hpp>
#include <llm/LLMClient.hpp>

#include <thread>
#include <chrono>

int main() {
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    
    auto process = jarvis::process::BaseProcess::Create();
    process->Start("ollama serve");

    jarvis::http::Client http;
    jarvis::llm::Client llm(http);

    while (!llm.IsAvailable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Ollama pronto!\n";

    bool running = true;
    while (running) {
        std::cout << "Voce > ";

        std::string prompt;
        std::getline(std::cin, prompt);

        if (prompt == "stop") {
            running = false;
            break;
        }

        std::cout << "AI > ";
        llm.Generate(prompt, [](const std::string& content) {
            std::cout << content << std::flush;
        });

        std::cout << '\n';
    }

    std::cout << "Finalizado Ollama\n";

    return 0;
}