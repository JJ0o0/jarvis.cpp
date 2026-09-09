#include <process/BaseProcess.hpp>

#ifdef _WIN32
	#include <process/platform/WindowsProcess.hpp>
#endif

#include <memory>

namespace jarvis::process {
	std::unique_ptr<BaseProcess> BaseProcess::Create() {
		#ifdef _WIN32
			return std::make_unique<WindowsProcess>();
		#endif
	}
}