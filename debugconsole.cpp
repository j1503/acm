#include "debugconsole.hpp"

#include <Windows.h>

debug::console::console()
{
	AllocConsole();
	freopen_s(&this->fp, "CONOUT$", "w", stdout);
	freopen_s(&this->fp, "CONOUT$", "w", stderr);
	freopen_s(&this->fp, "CONIN$", "r", stdin);
	SetConsoleTitle("ACM Debug");
}

debug::console::~console()
{
	HWND chwnd = GetConsoleWindow();
	fclose(this->fp);
	FreeConsole();
	PostMessage(chwnd, WM_CLOSE, 0, 0);
}