// Folly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "TestingServicesClient.h"

#include "ControlledFuture.h"
#include "ControlledMicroLock.h"
//#include <folly/ctrlfutures/Future.h>
#include <folly/executors/ThreadedExecutor.h>

int x = 1;
int y = 2;
int z = 4;
int balance = x;


bool depositDone = false;
bool withdrawDone = false;

using namespace folly;
using namespace std;


TestingServicesClient* _client = Helpers::CreateTestingServices(HTTP, "localhost", 8080, "");
ControlledMicroLock m1;

int foo(Unit) {
	std::cout << "foo()" << std::endl;
	return 23;
}

int foo_1(int x) {
	std::cout << "foo(" << x << ")" << std::endl;
	return (x+1);
}

Unit do_both(Unit)
{
	// m1.lock();
	std::cout << "do_both" << std::endl;
	// TestingServicesClient* socket = Helpers::GetTestingServices();
	_client->ContextSwitch();
	if (depositDone && withdrawDone)
	{
		// socket->Assert(balance == (x - y) - z, "Bug found!");
	}
	// m1.unlock();

	return folly::Unit();
}

Unit do_deposit(Unit)
{
	// m1.lock();
	std::cout << "do_deposit" << std::endl;
	_client->ContextSwitch();
	balance += y;
	depositDone = true;
	// m1.unlock();

	return folly::Unit();
}

Unit do_withdraw(Unit)
{
	std::cout << "do_withdraw" << std::endl;
	balance -= z;
	withdrawDone = true;

	return folly::Unit();
}

void testing() {

	// ControlledMicroLock m1;

	// cout << "Am -i called here> " << endl;

	// TestingServicesClient* socket = Helpers::CreateTestingServices(HTTP, "localhost", 8080, "");
	// TestingServicesClient* socket = Helpers::CreateTestingServices(HTTP, "10.168.90.87", 8080, "Q52SAUDV");

	/* folly::ThreadedExecutor executor;

	Promise<Unit> p1, p2, p3;

	Future<Unit> f1 = p1.getSemiFuture().via(&executor);
	auto f2 = move(f1).thenValueInline(do_both);

	Future<Unit> f3 = p2.getSemiFuture().via(&executor);
	auto f4 = move(f3).thenValueInline(do_deposit).thenValueInline(do_deposit);//  .thenValueInline(do_deposit).thenValueInline(do_deposit);

	Future<Unit> f5 = p3.getSemiFuture().via(&executor);
	auto f6 = move(f5).thenValueInline(do_withdraw);// .thenValueInline(do_withdraw).thenValueInline(do_withdraw).thenValueInline(do_withdraw);

	/* Future<Unit> f7 = Future<Unit>().via(&executor);
	auto f8 = move(f7).thenValueInline(foo);

	Promise<int> p4;
	Future<INT> f9 = NULL;
	f9 = p4.getFuture();
	// Future<Unit> f10();

	p1.setValue();
	p2.setValue();
	p3.setValue();

	socket->WaitForMainTask();

	move(f2).get();
	move(f4).get();
	move(f6).get(); */

	folly::ThreadedExecutor executor;
	/*ControlledFuture<Unit> d1 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> d3 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> d5 = ControlledFuture<Unit>().via(&executor);
	ControlledFuture<Unit> d7 = ControlledFuture<Unit>().via(&executor);
	// ControlledFuture<Unit> d9 = ControlledFuture<Unit>().via(&executor);
	auto d2 = move(d1).thenValueInline(foo);

	//ControlledFuture<Unit> d3 = ControlledFuture<Unit>().via(&executor);
	auto d4 = move(d3).thenValueInline(do_both).thenValueInline(do_both).thenValueInline(do_deposit).thenValueInline(do_deposit);

	//ControlledFuture<Unit> d5 = ControlledFuture<Unit>().via(&executor);
	auto d6 = move(d5).thenValueInline(foo);

	//ControlledFuture<Unit> d7 = ControlledFuture<Unit>().via(&executor);
	auto d8 = move(d7).thenValueInline(do_deposit).thenValueInline(do_both).thenValueInline(do_deposit).thenValueInline(do_both); */

	/* auto d10 = move(d9).thenValueInline([](Unit) {
		cout << "Test2" << endl;
		return foo_1(6);
		}); */

	// socket->WaitForMainTask();

	ControlledFuture<Unit> d1 = ControlledFuture<Unit>().via(&executor);
	auto d2 = move(d1).thenValueInline(do_withdraw).thenValue(do_withdraw).thenValueInline(do_withdraw);

	ControlledPromise<Unit> pr1,pr2,pr3;
	ControlledFuture<Unit> f1 = pr1.getFuture().via(&executor);
	ControlledFuture<Unit> f2 = pr2.getFuture().via(&executor);
	ControlledFuture<Unit> f3 = pr3.getFuture().via(&executor);


	auto z1 = move(f1).thenValueInline(do_both).thenValueInline(do_deposit);
	auto z2 = move(f2).thenValue(do_deposit).thenValueInline(do_both);
	auto z3 = move(f3).thenValueInline(do_both).thenValue(do_deposit);

	ControlledPromise<int> pr4;
	ControlledFuture<int> f4 = pr4.getFuture().via(&executor);
	
	auto z4 = move(f4).thenValue(foo_1).thenValueInline(foo_1).thenValue(foo_1).thenValueInline(foo_1);// .thenValueInline(foo_1);

	pr1.setValue();
	pr2.setValue();
	pr3.setValue();
	pr4.setValue(2);

	_client->WaitForMainTask();

	move(d2).get();
	move(z1).get();
	move(z2).get();
	move(z3).get();
	// move(z4).get();
	
	 while (!(z4).isReady()) {

	}

	std::cout << "THE VALUE --------------" << z4.value() << std::endl;

	move(z4).get(); 

	/* std::cout << "THE VALUE --------------" << (d2.fut).value() << std::endl;
	// move(d2.fut).get();
	move(d2).get();
	std::cout << "2" << endl;
	move(d4).get();
	std::cout << "3" << endl;
	move(d6).get();
	std::cout << "4" << endl;
	move(d8).get(); */
	// move(d4.fut).get();



	/* ControlledFuture<Unit> d3 = ControlledFuture<Unit>().via(&executor);
	auto d4 = move(d3).thenValueInline(do_deposit);

	ControlledFuture<Unit> d5 = ControlledFuture<Unit>().via(&executor);
	auto d6 = move(d5).thenValueInline(do_withdraw); */

}

int main()
{
	for (int i = 0; i < 1; i++) {
		testing();
	}
}

// typedef MicroLock ControlledMicroLock;

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
