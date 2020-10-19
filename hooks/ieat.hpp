#ifndef ACM_HOOKS_IEAT_HPP
#define ACM_HOOKS_IEAT_HPP
#pragma once

#include <vector>
#include <Windows.h>
#include <DbgHelp.h>

// import/export address table hooks
namespace hooks {
	// import address table
	class iat {
	public:
		iat() = default;
		/**
		* @param imageBase: base address of the module where the hooking interface will be installed
		*/
		iat(void* imageBase);
		/**
		* @param imageName: name of the image where the hooking interface will be installed (e.g. abc.exe) 
		*/
		iat(const char* imageName);
		/**
		* @param moduleName: name of the module (e.g. USER32.dll)
		* @param functionName: name of the exported function in the module
		* @param callback: function pointer to hooked function
		* @return index on success, -1 on failure
		*/
		int32_t hook(const char* moduleName, const char* functionName, void * callback);
		/**
		* @param index: index into the import address table
		* @param hk: function pointer to hooked function
		* @return true on success, false on failure
		*/
		bool hook(size_t index, void* hk);
		/**
		* @param moduleName: name of the module (e.g. USER32.dll)
		* @param functionName: name of the exported function in the module
		* @return true on success, false on failure
		*/
		bool restore(const char* moduleName, const char* functionName);
		/**
		* @param index: index into the import address table
		* @return true on success, false on failure
		*/
		bool restore(size_t index);

		/**
		*  original function getters
		*/
		template<class RET, class ... ARGS> 
		[[nodiscard]] inline auto getOriginalCdecl(const char * moduleName, const char * functionName) const {
			if (auto i = this->findIndex(moduleName, functionName); i != -1) {
				return reinterpret_cast<RET(__cdecl*)(ARGS...)>(this->original[i]);
			}
			return reinterpret_cast<RET(__cdecl*)(ARGS...)>(0);
		}

		template<class RET, class ... ARGS>
		[[nodiscard]] inline auto getOriginalCdecl(size_t index) const {
			return reinterpret_cast<RET(__cdecl*)(ARGS...)>(this->original[index]);
		}

		template<class RET, class ... ARGS>
		[[nodiscard]] inline auto getOriginalStdcall(const char* moduleName, const char* functionName) const {
			if (auto i = this->findIndex(moduleName, functionName); i != -1) {
				return reinterpret_cast<RET(__stdcall*)(ARGS...)>(this->original[i]);
			}
			return reinterpret_cast<RET(__stdcall*)(ARGS...)>(0);
		}

		template<class RET, class ... ARGS>
		[[nodiscard]] inline auto getOriginalStdcall(size_t index) const {
			return reinterpret_cast<RET(__stdcall*)(ARGS...)>(this->original[index]);
		}

		~iat();
	private:
		int32_t findIndex(const char* moduleName, const char* functionName) const;
	private:
		uintptr_t imageBase = 0u;

		uintptr_t ** iatbase;
		IMAGE_IMPORT_DESCRIPTOR * importbase;

		std::vector<bool> active;
		std::vector<uintptr_t*> original;
	};
}

#endif