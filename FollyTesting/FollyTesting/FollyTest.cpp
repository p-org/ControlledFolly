// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "NekaraCppClient.h"

// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/SmallLocks.h>

bool sh_t1 = false;
bool sh_t2 = false;

using namespace folly;
using namespace std;

NekaraCppClient* _client = Helpers::CreateTestingServices(HTTP, "localhost", 8080, "");
MicroLock ml;

// Folly represents void as Unit
Unit func_1(Unit)
{
	_client->StartThread(1);
	ml.lock();
	std::cout << "Lock given to Thread(1)" << std::endl;
	if (sh_t1 && sh_t2)
	{
		assert(false, "Bug found!");
	}
	ml.unlock();
	_client->EndThread(1);
	return folly::Unit();
}

Unit func_2(Unit)
{
	_client->StartThread(2);
	ml.lock();
	std::cout << "Lock given to Thread(2)" << std::endl;
	sh_t1 = true;
	ml.unlock();
	_client->EndThread(2);
	return folly::Unit();
}

Unit func_3(Unit)
{
	_client->StartThread(3);
	ml.lock();
	std::cout << "Lock given to Thread(3)" << std::endl;
	sh_t2 = true;
	ml.unlock();
	_client->EndThread(3);
	return folly::Unit();
}



int main()
{
	folly::ThreadedExecutor executor;
	_client->CreateThread();
	Future<Unit> f1 = Future<Unit>().via(&executor);
	_client->CreateThread();
	Future<Unit> f2 = Future<Unit>().via(&executor);
	_client->CreateThread();
	Future<Unit> f3 = Future<Unit>().via(&executor);


	auto d1 = move(f1).thenValue(func_1);
	auto d2 = move(f2).thenValue(func_2);
	auto d3 = move(f3).thenValue(func_3);

	_client->WaitForMainTask();

	move(d1).get();
	move(d2).get();
	move(d3).get();
}


