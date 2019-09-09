#pragma once
#include<Windows.h>

class Card
{
public:
	Card(LPWSTR sCardData);
	
	HRESULT GetCardID(LPWSTR* sCardID);

	~Card();

private:
	LPWSTR _sCardID;
};