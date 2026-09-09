#pragma once

#include <functional>
#include <string>
#include <vector>

namespace jarvis::http {
	class Client {
	public:
		Client();
		~Client();

		std::string Get(const std::string& url);

		using ResponseCallback = std::function<void(const std::string&)>;
		std::string Post(
			const std::string& url, 
			const std::string& body,
			const std::vector<std::string>& headers,
			ResponseCallback callback
		);
	};
}