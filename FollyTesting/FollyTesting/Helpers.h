#pragma once
#include <string>
#include <mutex>
#include "NekaraCppClient.h"
#ifndef Helpers_h
#define Helpers_h

class NekaraCppClient;

class Helpers
{
private:
	static std::mutex mu;
	static int count;
public:
	static int Count();
	static int RandomInt(int maxValue = 1000);
	static std::string RandomStr(int length = 10);
	static void Printingstuff(std::string counter, std::string func, std::string id, std::string state);
	static void Printres(std::string reqID);
	static void Printreserror(std::string reqID, std::string errormsg);
	static NekaraCppClient* CreateTestingServices(Transport transport, std::string host, int port, std::string sess);
	static NekaraCppClient* GetTestingServices();
};

#endif // !Helpers_h


