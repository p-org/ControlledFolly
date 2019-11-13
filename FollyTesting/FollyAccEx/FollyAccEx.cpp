// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/* #define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES */
#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>

int x = 1;
int y = 2;
int z = 4;
int balance = x;


bool depositDone = false;
bool withdrawDone = false;

using namespace folly;
using namespace std;


Unit do_both(Unit)
{
	if (depositDone && withdrawDone)
	{
		assert(balance == (x - y) - z, "Bug found!");
	}

	return folly::Unit();
}

Unit do_deposit(Unit)
{
	balance += y;
	depositDone = true;

	return folly::Unit();
}

Unit do_withdraw(Unit)
{
	balance -= z;
	withdrawDone = true;

	return folly::Unit();
}


int main()
{
	folly::ThreadedExecutor executor;
	Future<Unit> f1 = Future<Unit>().via(&executor);
	Future<Unit> f2 = Future<Unit>().via(&executor);
	Future<Unit> f3 = Future<Unit>().via(&executor);


	auto d1 = move(f1).thenValue(do_both);
	auto d2 = move(f2).thenValue(do_deposit);
	auto d3 = move(f3).thenValue(do_withdraw);


	move(d1).get();
	move(d2).get();
	move(d3).get();
}
