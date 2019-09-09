#pragma once

#include<Windows.h>
#include"Card.h"
class ICardHandler
{
public:
	virtual HRESULT HandleCard(Card* card) = 0;
};