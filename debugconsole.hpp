#ifndef ACM_DEBUG_CONSOLE_HPP
#define ACM_DEBUG_CONSOLE_HPP
#pragma once

#ifdef _DEBUG
#include <memory>

namespace debug {
	class console {
	public:
		console();
		~console();
	private:
		FILE* fp;
	};
}

namespace globals {
	inline std::unique_ptr<debug::console> DebugConsole;
}

#endif

#endif