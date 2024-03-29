#include "stdafx.h"
#include "CardReader.h"
#include <strsafe.h>
#include <Shlwapi.h>

/*Class member functions cannot be used for creating
thread, so we should use this static function to do that*/
DWORD CardReader::ThreadStart(PVOID p)
{
	CardReader* pCardReader = static_cast<CardReader*>(p);
	return pCardReader->CardReaderProc();
}

CardReader::CardReader() :
	_pHandler(nullptr),
	_sSerialPort(const_cast<LPWSTR>(L"")),
	_bBreakFlag(FALSE),
	_bEndFlag(FALSE)
{

}

CardReader::CardReader(LPWSTR sSerialPort, ICardHandler* pHandler = nullptr) :
	_pHandler(pHandler),
	_sSerialPort(sSerialPort),
	_bBreakFlag(FALSE),
	_bEndFlag(FALSE)
{
	CreateThread(NULL, 0, &ThreadStart, this, 0, NULL);
}


HRESULT CardReader::GetCardHandler(ICardHandler** ppHandler)
{
	if (_pHandler != nullptr)
	{
		*ppHandler = _pHandler;
		return S_OK;
	}
	else
	{
		*ppHandler = nullptr;
		return E_NOINTERFACE;
	}

}

HRESULT CardReader::GetSerialPort(LPWSTR* psSerialPort)
{
	*psSerialPort = _sSerialPort;
	return S_OK;
}

DWORD CardReader::CardReaderProc()
{

	LPWSTR sComPort;
	if (SUCCEEDED(GetSerialPort(&sComPort)))
	{
		
		HANDLE hCom = CreateFile(sComPort, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		DCB dcb;
		GetCommState(hCom, &dcb);
		dcb.BaudRate = CBR_9600;
		dcb.ByteSize = 8;
		dcb.fParity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		SetCommState(hCom, &dcb);

		COMMTIMEOUTS _cTimeouts = { 0, 0, 20, 0, 0 };
		SetCommTimeouts(hCom, &_cTimeouts);
		

		//read cards until broken
		while (!_bBreakFlag)
		{
			BOOL bCardRead = FALSE; //Flag that signals if card was read
			CHAR c;
			DWORD dSize;
			WCHAR wc = 0;

			//waiting for data from card reader
			while (!_bBreakFlag && !bCardRead)
			{
				dSize = 1;
				ReadFile(hCom, &c, dSize, &dSize, NULL);

				//if any data recieved
				if (dSize > 0)
				{
					mbtowc(&wc, &c, 1);
					bCardRead = TRUE;
				}
			}
			//if previous loop was end because card read, not by manually stop
			if (bCardRead && !_bBreakFlag)
			{
				bCardRead = FALSE;
				LPWSTR sCardData = new WCHAR[100]();
				sCardData[0] = wc;
				for (DWORD i = 1; !bCardRead && !_bBreakFlag;)
				{
					dSize = 1;
					ReadFile(hCom, &c, dSize, &dSize, NULL);

					if (dSize > 0)
					{
						mbtowc(&wc, &c, 1);
						sCardData[i] = wc;
						if (c == '\n') bCardRead = TRUE;
						i++;
					}
					Sleep(15);
				}
				//Check if card was removed		
				if (StrCmpW(sCardData, L"No card\n"))
				{
					Card* pCard = new Card(sCardData);
					ICardHandler* pCardHandler;
					if (SUCCEEDED(GetCardHandler(&pCardHandler)))
					{
						pCardHandler->HandleCard(pCard);
					}
				}
				delete sCardData;
			}
		}
		CloseHandle(hCom);
		_bEndFlag = TRUE;
		return 0;
	}
	else
	{
		_bEndFlag = TRUE;
		return 1;
	}


}

HRESULT CardReader::BreakReading()
{
	_bBreakFlag = TRUE;
	return S_OK;
}

VOID CardReader::WaitForEnd()
{
	while (!_bEndFlag) {}
}

CardReader::~CardReader()
{
	BreakReading();
	WaitForEnd();
}
