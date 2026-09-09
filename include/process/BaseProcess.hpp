#pragma once

#include <string>
#include <memory>

namespace jarvis::process {
	class BaseProcess {
	public:
		virtual ~BaseProcess() = default;

		static std::unique_ptr<BaseProcess> Create();

		virtual bool Start(const std::string& application) = 0;
		virtual void Stop() = 0;
	};
}