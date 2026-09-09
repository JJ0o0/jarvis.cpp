#include <process/platform/WindowsProcess.hpp>

#include <iostream>
#include <string>

namespace {
	std::wstring StringToWide(const std::string& string) {
		if (string.empty()) return {};

		int neededSize = MultiByteToWideChar(
			CP_UTF8, 0,
			&string[0], static_cast<int>(string.size()),
			nullptr, 0
		);

		std::wstring wideString(neededSize, 0);
		MultiByteToWideChar(
			CP_UTF8, 0,
			&string[0], static_cast<int>(string.size()),
			&wideString[0], neededSize
		);

		return wideString;
	}
}

namespace jarvis::process {
	WindowsProcess::~WindowsProcess() { Stop(); }

	bool WindowsProcess::Start(const std::string& application) {
		std::wstring applicationWide = StringToWide(application);

		SECURITY_ATTRIBUTES securityAttributes { sizeof(securityAttributes) };
		securityAttributes.bInheritHandle = true;

		HANDLE nulHandle = CreateFileW(
			L"NUL",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			&securityAttributes,
			OPEN_EXISTING,
			0,
			nullptr
		);

		STARTUPINFOW startupInfo { sizeof(startupInfo) };
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;
		startupInfo.hStdOutput = nulHandle;
		startupInfo.hStdError = nulHandle;

		bool result = CreateProcessW(
			nullptr,
			applicationWide.data(),
			nullptr, nullptr,
			false, 0,
			nullptr, nullptr,
			&startupInfo, &m_processInfo
		);

		CloseHandle(nulHandle);

		return result;
	}

	void WindowsProcess::Stop() {
		if (!m_processInfo.hProcess) return;

		DWORD result = WaitForSingleObject(m_processInfo.hProcess, 0);
		if (result == WAIT_TIMEOUT) {
			if (!TerminateProcess(m_processInfo.hProcess, 0)) {
				std::cerr << "Processo " << m_processInfo.dwProcessId << " nao foi fechado corretamente!\n";
			}
		}

		CloseHandle(m_processInfo.hProcess);
		CloseHandle(m_processInfo.hThread);

		m_processInfo.hProcess = nullptr;
		m_processInfo.hThread = nullptr;
	}
}