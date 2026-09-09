#include <llm/Conversation.hpp>
#include <iostream>

namespace jarvis::llm {
	void Conversation::AddMessage(const Message& msg) {
		m_messages.push_back(msg);
	}

	void Conversation::Clear() {
		m_messages.clear();
	}
}