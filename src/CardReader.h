#pragma once

#include<Windows.h>
#include"ICardHandler.h"


class CardReader
{
public:
	static DWORD ThreadStart(PVOID p);

	CardReader();
	CardReader(LPWSTR sSerialPort, ICardHandler* pHandler);
	HRESULT GetCardHandler(ICardHandler** ppHandler);
	HRESULT GetSerialPort(LPWSTR* psSerialPort);

	DWORD CardReaderProc();

	~CardReader();

private:
	HRESULT BreakReading();

	VOID WaitForEnd();

private:
	volatile BOOL _bBreakFlag;//If this flag is true the thread will try to end as soon as possible
	volatile BOOL _bEndFlag;//Flag shows if thread ended

	ICardHandler* _pHandler;
	LPWSTR _sSerialPort;
};
