#pragma once

std::wstring int2str(const int i);
std::wstring flt2str(const float f);

inline void debug(const wchar_t* str)
{
	::OutputDebugStringW(str);
	::OutputDebugStringW(L"\n");
}

inline void debug(const std::wstring& str)
{
	debug(str.c_str());
}

inline void debug(const int i)
{
	debug(int2str(i));
}

