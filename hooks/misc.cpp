#include "misc.hpp"

void hooks::makeRelativeJump(void* from, void* to)
{
	// formula: destination - (source + sizeof(instruction))
	unsigned int relAddr = (unsigned char*)to - ((unsigned char*)from + 5);
	*((unsigned char*)from) = 0xE9;
	*(unsigned int*)((unsigned char*)from + 1) = relAddr;
}

void hooks::makeRelativeCall(void* from, void* to)
{
	// formula: destination - (source + sizeof(instruction))
	unsigned int relAddr = (unsigned char*)to - ((unsigned char*)from + 5);
	*((unsigned char*)from) = 0xE8;
	*(unsigned int*)((unsigned char*)from + 1) = relAddr;
}

void hooks::makeExceptionByZero(void* addr)
{
	*(unsigned char*)addr = 0xA1;
	*(unsigned int*)((unsigned char*)addr + 1) = 0x0;
}