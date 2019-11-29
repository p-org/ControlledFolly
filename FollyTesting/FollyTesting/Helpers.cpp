#include "Helpers.h"
#include "string"
#include <cstdlib>
#include <time.h> 
#include <iostream>
#include <cassert>
#include "NekaraCppClientConfig.h"

std::mutex Helpers::mu;
int Helpers::count;

int Helpers::Count()
{
	static int count = 1;
	return count++;
}

int Helpers::RandomInt(int maxValue)
{
	static int count = 1;
	srand(time(0));
	count++;
	return maxValue * (rand() / (RAND_MAX + 1.0)) + count;
}

std::string Helpers::RandomStr(int length)
{
	/* static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	char s[10] ;

	for (int i = 0; i < length; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	std::string str(s); */

	std::string str = "ABC" + std::to_string(Helpers::RandomInt()) + "HGF" + std::to_string(Helpers::RandomInt()) + "Z";

	return str;
}

void Helpers::Printingstuff(std::string counter, std::string func, std::string id, std::string state)
{
	Helpers::mu.lock();
	// std::cout << counter << "\t" << func << "(" << id << ")" << "\t" << state << " Thread ID: " << std::this_thread::get_id() << std::endl;
	Helpers::mu.unlock();
}

void Helpers::Printres(std::string reqID)
{
	Helpers::mu.lock();
	std::cout << "--> Got Response to request: " << reqID << " error: False." << " Thread ID: " << std::this_thread::get_id() << std::endl;
	Helpers::mu.unlock();
}

void Helpers::Printreserror(std::string reqID, std::string errormsg)
{
	Helpers::mu.lock();
	std::cout << "--> Got Response to request: " << reqID << " error: True." << " Thread ID: " << std::this_thread::get_id() << std::endl;
	std::cout << "The Error: " << errormsg << std::endl;
	Helpers::mu.unlock();
	throw 1;
}

 /* NekaraCppClient*  Helpers::CreateTestingService() {
	NekaraCppClientConfig omc(HTTP, "localhost", 8080);
	// Helpers::_socket = new NekaraCppClient(omc);

	/* static NekaraCppClient* _socket;
	static bool flag1 = true;
	if (flag1) {
		NekaraCppClientConfig omc(HTTP, "localhost", 8080);
		_socket = new NekaraCppClient(omc);
	}
	return _socket; 

	return Helpers::_socket;
} */

NekaraCppClient*  Helpers::CreateTestingServices(Transport transport, std::string host, int port, std::string sess = "") {
	static NekaraCppClient* _socket;
	Helpers::count = 0;

	if (host.compare("GetTestingServices") != 0)
	{
		NekaraCppClientConfig omc(transport, host, port, sess);
		_socket = new NekaraCppClient(omc);
	}
	/* if (flag1) {
		NekaraCppClientConfig omc(transport, host, port);
		_socket = new NekaraCppClient(omc);
		flag1 = false;
	} */
	return _socket;
}

NekaraCppClient*  Helpers::GetTestingServices() {
	// return Helpers::_socket;
	/* static NekaraCppClient* _socket;
	static bool flag1 = true;
	if (flag1) {
		NekaraCppClientConfig omc(HTTP, "localhost", 8080);
		_socket = new NekaraCppClient(omc);
		flag1 = false;
	}
	return _socket; */
	return Helpers::CreateTestingServices(HTTP, "GetTestingServices", 8080);
	// return Helpers::_socket;
} 