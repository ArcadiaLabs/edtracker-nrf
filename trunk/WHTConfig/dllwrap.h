#pragma once

class dll
{
private:
	HMODULE	hDll;

public:
	dll()
		: hDll(0)
	{}

	~dll()
	{
		release();
	}

	void release()
	{
		if (hDll != 0)
		{
			::FreeLibrary(hDll);
			hDll = 0;
		}
	}

	bool empty() const
	{
		return hDll == 0;
	}

	bool load(const wchar_t* dll_name)
	{
		release();

		hDll = ::LoadLibrary(dll_name);
		
		return hDll != 0;
	}

	template <typename ProcType>
	bool get_proc(ProcType& p, const char* proc_name)
	{
		p = (ProcType) ::GetProcAddress(hDll, proc_name);
		return p != 0;
	}
};
