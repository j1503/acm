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
		[[nodiscard]] void* getGateway() const noexcept;
		template<class RET, class ...ARGS>
		[[nodiscard]] inline auto getGatewayCdecl() const noexcept {
			return reinterpret_cast<RET(__cdecl*)(ARGS...)>(this->gateway);
		}
		template<class RET, class ...ARGS>
		[[nodiscard]] inline auto getGatewayStdcall() const noexcept {
			return reinterpret_cast<RET(__stdcall*)(ARGS...)>(this->gateway);
		}
	private:
		uintptr_t src;
		uintptr_t dst;
		void* gateway;
		size_t length;
		bool active;
	};
}

#endif