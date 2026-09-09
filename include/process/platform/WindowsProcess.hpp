#pragma once

#include <process/BaseProcess.hpp>

#include <windows.h>

#include <string>

namespace jarvis::process {
	class WindowsProcess : public BaseProcess {
	public:
		~WindowsProcess() override;

		bool Start(const std::string& application) override;
		void Stop() override;
	private:
		PROCESS_INFORMATION m_processInfo { 0 };
	};
}