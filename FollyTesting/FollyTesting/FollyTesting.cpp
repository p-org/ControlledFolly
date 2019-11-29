// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "NekaraCppClient.h"

#include "ControlledFuture.h"
#include "ControlledMicroLock.h"
#include <folly/executors/ThreadedExecutor.h>

bool sh_t1 = false;
bool sh_t2 = false;

using namespace folly;
using namespace std;

NekaraCppClient* _client = Helpers::CreateTestingServices(HTTP, "localhost", 8080, "");
ControlledMicroLock ml;

// Folly represents void as Unit
Unit func_1(Unit)
{

	ml.lock();
	std::cout << "Lock given to Thread(1)" << std::endl;
	if (sh_t1 && sh_t2)
	{
		_client->Assert(false, "Bug found!");
	}
	ml.unlock();
	return folly::Unit();
}

Unit func_2(Unit)
{

	ml.lock();
	std::cout << "Lock given to Thread(2)" << std::endl;
	sh_t1 = true;
	ml.unlock();

	return folly::Unit();
}

Unit func_3(Unit)
{

	ml.lock();
	std::cout << "Lock given to Thread(3)" << std::endl;
	sh_t2 = true;
	ml.unlock();

	return folly::Unit();
}



int main()
{
	folly::ThreadedExecutor executor;
	ControlledFuture<Unit> f1 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> f2 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> f3 = ControlledFuture<Unit>().via(&executor);


	auto d1 = move(f1).thenValue(func_1);
	auto d2 = move(f2).thenValue(func_2);
	auto d3 = move(f3).thenValue(func_3);

	_client->WaitForMainTask();

	move(d1).get();
	move(d2).get();
	move(d3).get();
}

