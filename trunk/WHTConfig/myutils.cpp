#include "stdafx.h"

std::wstring int2str(const int u)
{
	const int BUFF_SIZE = 20;
	wchar_t buffer[BUFF_SIZE];
	swprintf_s(buffer, BUFF_SIZE, L"%u", u);
	return buffer;
}
