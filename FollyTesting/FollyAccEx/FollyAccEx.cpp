// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/SmallLocks.h>

bool sh_t1 = false;
bool sh_t2 = false;

using namespace folly;
using namespace std;


MicroLock ml;

// Folly represents void as Unit
Unit func_1(Unit)
{

	ml.lock();
	std::cout << "Lock given to Thread(1)" << std::endl;
	if (sh_t1 && sh_t2)
	{
		assert(false, "Bug found!");
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
