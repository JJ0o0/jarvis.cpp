#pragma once

#include <string>

namespace jarvis::llm {
	enum class MessageRole {
		User,
		Assistant
	};

	struct Message {
		MessageRole Role = MessageRole::User;
		std::string Content;
	};
}