#pragma once

#if defined(_WIN32) && defined(NDEBUG)
#include <Tether/Platform/Win32Window.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define LEGENDENGINE_MAIN int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#else
#define LEGENDENGINE_MAIN int main()
#endif