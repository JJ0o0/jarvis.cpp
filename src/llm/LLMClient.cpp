#include <llm/LLMClient.hpp>
#include <llm/Message.hpp>

#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <string>

namespace jarvis::llm {
	Client::Client(http::Client& httpClient) 
		: http(httpClient) {
	}

	std::string Client::Generate(const std::string& prompt, StreamCallback callback) {
		m_conversation.AddMessage(Message{
			.Role = MessageRole::User,
			.Content = prompt
		});

		nlohmann::json messages = nlohmann::json::array();
		for (const auto& msg : m_conversation.GetMessages()) {
			nlohmann::json msgJson;
			msgJson["role"] = msg.Role != MessageRole::Assistant ? "user" : "assistant";
			msgJson["content"] = msg.Content;

			messages.push_back(msgJson);
		}

		// FORMANDO JSON
		nlohmann::json request;
		request["model"] = "llama3.2:3b";
		request["messages"] = messages;
		request["stream"] = true;

		std::string dumpedRequest = request.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);

		// FAZENDO REQUISIÇÃO HTTP
		std::vector<std::string> headers{
			"Content-Type: application/json"
		};

		std::string answer;
		std::string streamBuffer;

		try {
			http.Post(
				"http://localhost:11434/api/chat",
				dumpedRequest,
				headers,
				[&](const std::string& chunk) {
					streamBuffer += chunk;

					while (true) {
						const std::size_t newlinePos = streamBuffer.find('\n');
						if (newlinePos == std::string::npos) break;

						const std::string line = streamBuffer.substr(0, newlinePos);
						streamBuffer.erase(0, newlinePos + 1);

						if (line.empty()) continue;

						try {
							const nlohmann::json response = nlohmann::json::parse(line);
							if ((response.is_object() && response.contains("message") && response["message"].is_object())) {
								const auto& msgObj = response["message"];
								if (msgObj.contains("content") && msgObj["content"].is_string()) {
									const std::string content = msgObj["content"].get<std::string>();
									answer += content;

									if (callback) callback(content);
								}
							}
						}
						catch (...) {
							continue;
						}
					}
				}
			);
		} catch (...) {
			std::cerr << "Excessão ativada no gerador do Client\n";
		}

		// PEGANDO RESPOSTA COMO JSON
		m_conversation.AddMessage({
			.Role = MessageRole::Assistant,
			.Content = answer
		});

		return answer;
	}

	bool Client::IsAvailable() {
		try {
			return !http.Get("http://localhost:11434").empty();
		} catch (...) {
			return false;
		}
	}
}