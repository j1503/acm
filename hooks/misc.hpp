#ifndef ACM_HOOKS_MISC_HPP
#define ACM_HOOKS_MISC_HPP
#pragma once

namespace hooks {
	void makeRelativeJump(void* from, void* to);
	void makeRelativeCall(void* from, void* to);
	void makeExceptionByZero(void* addr);
}

#endif