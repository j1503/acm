#ifndef ACM_HOOKS_VMT_HPP
#define ACM_HOOKS_VMT_HPP
#pragma once

#include <vector>

namespace hooks {
	class vmt {
	public:
		vmt() = default;
		/**
		* @brief initializes a vtable to be hookable by storing its original state, etc. Note that the destructor of this class restores all hooks,
		* so there is no need to explicitly call restore on all hooks
		* @param vTableBase: address of first element in vtable
		* @param virtualprotect: (default=true) should be set to false if the memory areas permission does not need
		* to be altered
		*/
		vmt(void* vtableBase, bool virtualprotect = true);
		~vmt();
		/**
		* @brief replaces function pointer at vtable[<index>] with <function>
		* @param index: index into the vtable
		* @param function: pointer to function
		*/
		void hook(const size_t index, void* function) noexcept;
		/**
		* @brief restores the function pointer at vtable[<index>] with the original address
		* @param index: index into the vtable
		*/
		void restore(const size_t index) noexcept;
		/**
		* @brief get the original function at vtable[index] as a function pointer
		*/
		template<const size_t index, class RET, class ...ARGS>
		[[nodiscard]] auto getOriginal() const noexcept {
			return reinterpret_cast<RET(__thiscall*)(void*, ARGS...)>(this->originalvTable[index]);
		}
		/**
		* @brief call the original function at vtable[index]
		* @param args: (variadic) arguments to function call
		*/
		template<const size_t index, class RET, class ...ARGS>
		auto callOriginal(ARGS...args) const noexcept {
			return this->getOriginal<index, RET, ARGS...>()(this->vtable, args...);
		}
		/**
		@brief returns amount of entries in the vtable
		@param vtableBase: pointer to the first element in vtable
		*/
		static size_t getVmtEntryCount(void* vtableBase);

	private:
		void* vtable;
		size_t length;
		bool virtualprotect;
		std::vector<bool> active;
		std::vector<uintptr_t> originalvTable;
	};
	class vmtswap {
	public:
		vmtswap() = default;
		vmtswap(void* instance);
		~vmtswap();
		void hook(const size_t index, void* function);
		void restore(const size_t index);
		/**
		* @brief get the original function at vtable[index] as a function pointer
		*/
		template<const size_t index, class RET, class ...ARGS>
		[[nodiscard]] auto getOriginal() const noexcept {
			return reinterpret_cast<RET(__thiscall*)(void*, ARGS...)>(this->originalvTableBase[index]);
		}
		/**
		* @brief call the original function at vtable[index]
		* @param args: (variadic) arguments to function call
		*/
		template<const size_t index, class RET, class ...ARGS>
		auto callOriginal(ARGS...args) const noexcept {
			return this->getOriginal<index, RET, ARGS...>()(this->classinstance, args...);
		}
		/**
		@brief returns amount of entries in the vtable
		@param vtableBase: pointer to the first element in vtable
		*/
		static size_t getVmtEntryCount(void* vtableBase);
	private:
		void* classinstance;
		uintptr_t originalvTableAddress;
		uintptr_t* originalvTableBase;
		size_t length;
		std::vector<bool> active;
		std::vector<uintptr_t> vtableswapped;
	};
}

#endif