#include <llm/LLMClient.hpp>
#include <llm/Message.hpp>

#include <nlohmann/json.hpp>
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

		std::string dumpedRequest = request.dump();

		// FAZENDO REQUISIÇÃO HTTP
		std::vector<std::string> headers{
			"Content-Type: application/json"
		};

		std::string answer;
		std::string streamBuffer;

		http.Post(
			"http://localhost:11434/api/chat",
			dumpedRequest,
			headers,
			[&](const std::string& chunk) {
				streamBuffer += chunk;

				while (streamBuffer.find('\n') != std::string::npos) {
					const std::size_t newlinePos = streamBuffer.find('\n');
					const std::string line = streamBuffer.substr(0, newlinePos);
					streamBuffer.erase(0, newlinePos + 1);

					try {
						const nlohmann::json response = nlohmann::json::parse(line);
						if (response.contains("message") && response["message"].contains("content")) {
							const std::string content = response["message"]["content"];
							answer += content;

							callback(content);
						}
					} catch (const nlohmann::json::parse_error&) {
						continue;
					}
				}
			}
		);

		// PEGANDO RESPOSTA COMO JSON
		m_conversation.AddMessage({
			.Role = MessageRole::Assistant,
			.Content = answer
		});

		return answer;
	}

	bool Client::IsAvailable() {
		return !http.Get("http://localhost:11434").empty();
	}
}