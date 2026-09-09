#pragma once

#include "Message.hpp"

#include <vector>

namespace jarvis::llm {
	class Conversation {
	public:
		void AddMessage(const Message& msg);
		void Clear();

		const std::vector<Message>& GetMessages() const { return m_messages; }
	private:
		std::vector<Message> m_messages{};
	};
}