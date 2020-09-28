#ifndef ACM_MEMORY_HPP
#define ACM_MEMORY_HPP
#pragma once

#include <memory>
#include <glm/mat4x4.hpp>
#include "sdk/entity.hpp"

namespace memory
{
	class memoryManager {
	public:
		memoryManager();
		~memoryManager();
		playerent* localPlayer;
		playerent** entityList;
		size_t* entityCount;
		glm::mat4* viewMatrix;
	};
	uintptr_t findPatternNaiive(const char* moduleName, const char* pattern) noexcept;
	uintptr_t findPatternBMH(const char* moduleName, const char* pattern) noexcept;
	bool isPatternUnique(const char* moduleName, const char* pattern) noexcept;
}

namespace globals {
	inline std::unique_ptr<memory::memoryManager> MemoryManager;
}
#endif