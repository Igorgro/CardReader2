#include "stdafx.h"
#include "Card.h"
#include <Windows.h>
#include <strsafe.h>
#include <Shlwapi.h>


Card::Card(LPWSTR sCardData) :
	_sCardID(nullptr)
{

	PCWSTR pStart = StrChrW(sCardData, L'[');
	if (pStart)
	{
		SIZE_T start = pStart - sCardData+1;
		
		PCWSTR pEnd = StrChrW(sCardData, L']');
		if (pEnd)
		{
			SIZE_T end = pEnd - sCardData-1;
			_sCardID = new WCHAR[end-start+2]();
			StringCchCopyNW(_sCardID, end-start+2, pStart+1, end-start+1);
		}
	}
}

HRESULT Card::GetCardID(LPWSTR* sCardID)
{
	if (_sCardID != nullptr)
	{
		*sCardID = _sCardID;
		return S_OK;
	}
	else
	{
		*sCardID = nullptr;
		return E_FAIL;
	}
}

Card::~Card()
{
	delete _sCardID;
}
