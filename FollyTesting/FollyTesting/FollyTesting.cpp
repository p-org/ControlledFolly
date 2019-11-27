// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "TestingServicesClient.h"

#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/SmallLocks.h>

bool depositDone = false;
bool withdrawDone = false;

using namespace folly;
using namespace std;


MicroLock ml;

// Folly represents void as Unit
Unit func_1(Unit)
{
	std::cout << "Execution given to Thread(1)" << std::endl;

	ml.lock();
	std::cout << "Lock given to Thread(1)" << std::endl;
	if (depositDone && withdrawDone)
	{
		// _client->Assert(false, "Bug found!");
	}
	ml.unlock();
	return folly::Unit();
}

Unit func_2(Unit)
{
	std::cout << "Execution given to Thread(2)" << std::endl;

	ml.lock();
	std::cout << "Lock given to Thread(2)" << std::endl;
	depositDone = true;
	ml.unlock();

	return folly::Unit();
}

Unit func_3(Unit)
{
	std::cout << "Execution given to Thread(3)" << std::endl;

	ml.lock();
	std::cout << "Lock given to Thread(3)" << std::endl;
	withdrawDone = true;
	ml.unlock();

	return folly::Unit();
}


void testing()
{
	folly::ThreadedExecutor executor;
	Future<Unit> f1 = Future<Unit>().via(&executor);
	Future<Unit> f2 = Future<Unit>().via(&executor);
	Future<Unit> f3 = Future<Unit>().via(&executor);


	auto d1 = move(f1).thenValue(func_1);
	auto d2 = move(f2).thenValue(func_2);
	auto d3 = move(f3).thenValue(func_3);

	move(d1).get();
	move(d2).get();
	move(d3).get();
}

int main()
{
	for (int i = 0; i < 1; i++)
	{
		testing();
	}
}

