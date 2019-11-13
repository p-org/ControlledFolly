#pragma once
#ifndef IClientMethods_h
#define IClientMethods_h
#include <iostream>
class IClientMethods
{
public:
	IClientMethods() {}
	virtual ~IClientMethods() {}
	virtual void SendRequest(std::string func, std::string json) = 0;
};

#endif // !IClientMethods_h

