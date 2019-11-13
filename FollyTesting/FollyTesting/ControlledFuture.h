
#pragma once 
#include "TestingServicesClient.h"
#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>



using namespace folly;
using namespace std;

template <class T>
class ControlledFuture {
public:
	Promise<T> p1;
	folly::Future<T> fut = this->p1.getFuture();
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
	ControlledFuture<T2> via(folly::Executor::KeepAlive<> executor)&& {
		ControlledFuture<T2> f1 = ControlledFuture<T2>(true);
		f1.flag = this->flag;
		f1.fut = (f1.fut).via(executor);
		f1.temp_fut = (f1.temp_fut).via(executor);
		return f1;
	}


	template <typename R, typename... Args>
	auto thenValue(R(&func)(Args...))&& {
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
		}
		return f1;
	}


	template <typename T2 = T>
	T2 get() {
		move(this->temp_fut).get();
		return move(this->fut).get();
	}

	// template <class T>
	bool isReady() const {
		return this->fut.isReady();
	}

	// template <class T>
	bool hasValue() const {
		return this->fut.hasValue();
	}

	// template <class T>
	bool hasException() const {
		return this->fut.hasException();
	}

	// template <class T>
	T& value()& {
		return this->fut.value();
	}

	// template <class T>
	T const& value() const& {
		return this->fut.value();
	}

	// template <class T>
	T&& value()&& {
		return this->fut.value();
	}

	// template <class T>
	T const&& value() const&& {
		return this->fut.value();
	}

	// template <class T>
	Try<T>& result()& {
		return this->fut.result();
	}

	// template <class T>
	Try<T> const& result() const& {
		return this->fut.result();
	}

	// template <class T>
	Try<T>&& result()&& {
		return this->fut.result();
	}

	// template <class T>
	Try<T> const&& result() const&& {
		return this->fut.result();
	}

	void cancel() {
		this->fut.cancel();
	}

	bool valid() const noexcept {
		this->fut.valid();
	}

	ControlledFuture<Unit> unit()&& {
		ControlledFuture<Unit> f1 = ControlledFuture<Unit>(false);
		f1.fut = std::move(*(this->fut)).then();
		return f1;
	}

	// template <class T>
	ControlledFuture<Unit> then()&& {
		ControlledFuture<Unit> f1 = ControlledFuture<Unit>(false);
		f1.fut = std::move(*(this->fut)).thenValue([](T&&) {});
		return f1;
	}

};