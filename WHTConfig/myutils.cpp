#include "stdafx.h"

std::wstring int2str(const int u)
{
	const int BUFF_SIZE = 20;
	wchar_t buffer[BUFF_SIZE];
	swprintf_s(buffer, BUFF_SIZE, L"%u", u);
	return buffer;
}

std::wstring int2flt(const float f)
{
	const int BUFF_SIZE = 32;
	wchar_t buff[BUFF_SIZE];
	swprintf_s(buff, sizeof(buff) / sizeof(wchar_t), L"%g", f);

	return buff;
}