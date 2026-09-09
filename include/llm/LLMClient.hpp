#pragma once

#include <http/HTTPClient.hpp>
#include "Conversation.hpp"

#include <functional>
#include <string>

namespace jarvis::llm {
	class Client {
	public:
		Client(http::Client& httpClient);

		using StreamCallback = std::function<void(const std::string&)>;
		std::string Generate(const std::string& prompt, StreamCallback callback);

		bool IsAvailable();
	private:
		http::Client& http;

		Conversation m_conversation;
	};
}