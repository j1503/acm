#ifndef ACM_HOOKS_BYTEPATCH_HPP
#define ACM_HOOKS_BYTEPATCH_HPP
#pragma once

#include <cinttypes>
#include <vector>

namespace hooks {
	class trampoline {
	public:
		trampoline() = default;
		trampoline(const char* moduleName, const char* functionName, void* dst, const size_t bytes);
		trampoline(void* src, void* dst, const size_t bytes);
		~trampoline();
		void hook() noexcept;
		void restore() noexcept;
		void* getGateway() const noexcept;
	private:
		uintptr_t src;
		uintptr_t dst;
		void* gateway;
		size_t length;
		bool active;
	};
}

#endif