// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "TestingServicesClient.h"

#include "ControlledFuture.h"
#include <folly/executors/ThreadedExecutor.h>

int x = 1;
int y = 2;
int z = 4;
int balance = x;


bool depositDone = false;
bool withdrawDone = false;

using namespace folly;
using namespace std;

// In Folly Void type is represented using Unit.
Unit do_both(Unit)
{
	TestingServicesClient* socket = Helpers::GetTestingServices();
	if (depositDone && withdrawDone)
	{
		socket->Assert(balance == (x - y) - z, "Bug found!");
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

void testing() {

	// Starting the testing services client. Only HTTP service supported at the moment.
	TestingServicesClient* socket = Helpers::CreateTestingServices(HTTP, "localhost", 8080, "");
	
	folly::ThreadedExecutor executor;
	ControlledFuture<Unit> f1 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> f2 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> f3 = ControlledFuture<Unit>().via(&executor);


	auto d1 = move(f1).thenValue(do_both);
	auto d2 = move(f2).thenValue(do_deposit);
	auto d3 = move(f3).thenValue(do_withdraw);


	socket->WaitForMainTask();

	move(d1).get();
	move(d2).get();
	move(d3).get();
}

int main()
{
	for (int i = 0; i < 1; i++) {
		testing();
	}
}
