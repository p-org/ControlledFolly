
#pragma once 
#include "TestingServicesClient.h"
#include "ControlledPromise.h"
#include <folly/futures/Future.h>
#include <folly/executors/ThreadedExecutor.h>

using namespace folly;
using namespace std;

template <typename T>
class ControlledFuture {
public:
	Promise<T> _promise;
	// ControlledPromise<T> _promise = ControlledPromise<T>();
	folly::Future<T> _future = this->_promise.getFuture();
	Promise<T> _t_promise;
	folly::Future<T> _t_future = this->_t_promise.getFuture();
	bool _flag = false;
	bool _flag_from_promise = false;
	int _count = 0;

	template <typename T2 = T>
	ControlledFuture(typename std::enable_if<std::is_same<folly::Unit, T2>::value>::type* p = nullptr)
	{
		// this->_future = this->_promise.getFuture();
		// std::cout << "c1-called" << std::endl;
	}

	ControlledFuture()
	{
		TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->CreateThread();
		this->_flag = true;
	}

	ControlledFuture(bool _flag)
	{

	}

	template <typename T2 = T>
	ControlledFuture<T2> via(folly::Executor::KeepAlive<> executor)&& {
		ControlledFuture<T2> _f1 = ControlledFuture<T2>(true);
		_f1._flag = this->_flag;
		_f1._flag_from_promise = this->_flag_from_promise;
		_f1._promise = move(this->_promise);
		_f1._future = (this->_future).via(executor);
		_f1._t_future = (this->_t_future).via(executor);

		return _f1;
	}


	template <typename R, typename... Args>
	auto thenValue(R(&func)(Args...))&& {
		ControlledFuture<R> _f1 = ControlledFuture<R>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::RandomInt();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				TestingServicesClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValue(&func);

				_f1._t_future = move(_f1._future).thenValue([&](R _x) {
					TestingServicesClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					this->_promise.setValue();
				}
		}
		else
		{
			this->_count = Helpers::RandomInt();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				TestingServicesClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread(); 
				_socket->StartThread(this->_count);
				return _x;
				}).thenValue(&func);

				_f1._t_future = move(_f1._future).thenValue([&](R _x) {
					TestingServicesClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}


	template <typename T2 = T>
	T2 get() {
		move(this->_t_future).get();
		return move(this->_future).get();
	}

	// template <class T>
	bool isReady() const {
		return this->_future.isReady();
	}

	// template <class T>
	bool hasValue() const {
		return this->_future.hasValue();
	}

	// template <class T>
	bool hasException() const {
		return this->_future.hasException();
	}

	// template <class T>
	T& value()& {
		return this->_future.value();
	}

	// template <class T>
	T const& value() const& {
		return this->_future.value();
	}

	// template <class T>
	T&& value()&& {
		return this->_future.value();
	}

	// template <class T>
	T const&& value() const&& {
		return this->_future.value();
	}

	// template <class T>
	Try<T>& result()& {
		return this->_future.result();
	}

	// template <class T>
	Try<T> const& result() const& {
		return this->_future.result();
	}

	// template <class T>
	Try<T>&& result()&& {
		return this->_future.result();
	}

	// template <class T>
	Try<T> const&& result() const&& {
		return this->_future.result();
	}

	void cancel() {
		this->_future.cancel();
	}

	bool valid() const noexcept {
		this->_future.valid();
	}

	ControlledFuture<Unit> unit()&& {
		ControlledFuture<Unit> _f1 = ControlledFuture<Unit>(false);
		_f1._future = std::move(*(this->_future)).then();
		return _f1;
	}

	// template <class T>
	ControlledFuture<Unit> then()&& {
		ControlledFuture<Unit> _f1 = ControlledFuture<Unit>(false);
		_f1._future = std::move(*(this->_future)).thenValue([](T&&) {});
		return _f1;
	}

};
