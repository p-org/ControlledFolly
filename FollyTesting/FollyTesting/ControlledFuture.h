
#pragma once
// #include <future> 
#include "TestingServicesClient.h"
#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>
// #include <folly/ctrlfutures/detail/Core.h>


using namespace folly;
using namespace std;

	template <class T>
	class ControlledFuture {
	public:
		Promise<T> p1;
		folly::Future<T> fut = this->p1.getFuture();
		// Promise<Unit> temp_promise;
		// folly::Future<Unit> temp_fut = this->temp_promise.getFuture();
		folly::Future<Unit> temp_fut;
		bool flag = false;
		int count = 0;

		template <typename T2 = T>
		ControlledFuture(typename std::enable_if<std::is_same<folly::Unit, T2>::value>::type* p = nullptr)
		{
			// this->fut = this->p1.getFuture();
			// std::cout << "c1-called" << std::endl;
		}

		ControlledFuture()
		{
			TestingServicesClient* socket = Helpers::GetTestingServices();
			socket->CreateThread();
			this->flag = true;
		}

		ControlledFuture(bool flag)
		{

		}

		template <typename T2 = T>
		ControlledFuture<T2> via(folly::Executor::KeepAlive<> executor) && {
			ControlledFuture<T2> f1 = ControlledFuture<T2>(true);
			f1.flag = this->flag;
			f1.fut = (f1.fut).via(executor);
			f1.temp_fut = (f1.temp_fut).via(executor);
			return f1;
		}


		template <typename R, typename... Args>
		auto thenValue(R(&func)(Args...))&& {
			// std::cout << "Testing:1" << std::endl;

			ControlledFuture<R> f1 = ControlledFuture<R>(false);

			if (this->flag)
			{
				this->count = Helpers::RandomInt();
				f1.fut = move(this->fut).thenValue([&](T) {
					TestingServicesClient* socket = Helpers::GetTestingServices();
					socket->StartThread(this->count);
					}).thenValue(&func);

					f1.temp_fut = move(f1.fut).thenValue([&](R) {
						TestingServicesClient* socket = Helpers::GetTestingServices();
						socket->EndThread(this->count);
						});

					this->p1.setValue();
			}
			else 
			{
				this->count = Helpers::RandomInt();
				f1.fut = move(this->temp_fut).thenValue([&](T) {
					TestingServicesClient* socket = Helpers::GetTestingServices();
					socket->CreateThread();
					socket->StartThread(this->count);
					}).thenValue(&func);

					f1.temp_fut = move(f1.fut).thenValue([&](R) {
						TestingServicesClient* socket = Helpers::GetTestingServices();
						socket->EndThread(this->count);
						}); 

					// this->temp_promise.setValue();
			}
			return f1;
		}


		template <typename T2 = T>
		T2 get() {

			// cout << "AM I called here?" << endl;
			// TestingServicesClient* socket = Helpers::GetTestingServices();
			// socket->ContextSwitch();
			// socket->EndThread(this->count);
			// move(this->temp_fut).get();
			return move(this->fut).get();
		}

	};