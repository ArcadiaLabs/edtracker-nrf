#include "stdafx.h"

std::wstring int2str(const int i)
{
	const int BUFF_SIZE = 20;
	wchar_t buffer[BUFF_SIZE];
	swprintf_s(buffer, BUFF_SIZE, L"%i", i);
	return buffer;
}

std::wstring flt2str(const float f)
{
	const int BUFF_SIZE = 32;
	wchar_t buff[BUFF_SIZE];
	swprintf_s(buff, sizeof(buff) / sizeof(wchar_t), L"%g", f);

	return buff;
}