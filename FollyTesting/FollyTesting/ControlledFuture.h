
#pragma once 
#include "NekaraCppClient.h"
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
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->CreateThread();
		this->_flag = true;
	}

	ControlledFuture(bool _flag)
	{

	}


	// Wrapper(s) for "via" API
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

	template <typename T2 = T>
	ControlledFuture<T2> via(Executor::KeepAlive<> executor, int8_t priority)&& {
		ControlledFuture<T2> _f1 = ControlledFuture<T2>(true);
		_f1._flag = this->_flag;
		_f1._flag_from_promise = this->_flag_from_promise;
		_f1._promise = move(this->_promise);
		_f1._future = (this->_future).via(executor, priority);
		_f1._t_future = (this->_t_future).via(executor, priority);

		return _f1;
	}

	template <typename T2 = T>
	ControlledFuture<T2> via(folly::Executor::KeepAlive<> executor) & {
		ControlledFuture<T2> _f1 = ControlledFuture<T2>(true);
		_f1._flag = this->_flag;
		_f1._flag_from_promise = this->_flag_from_promise;
		_f1._promise = move(this->_promise);
		_f1._future = (this->_future).via(executor);
		_f1._t_future = (this->_t_future).via(executor);

		return _f1;
	}

	template <typename T2 = T>
	ControlledFuture<T2> via(Executor::KeepAlive<> executor, int8_t priority) & {
		ControlledFuture<T2> _f1 = ControlledFuture<T2>(true);
		_f1._flag = this->_flag;
		_f1._flag_from_promise = this->_flag_from_promise;
		_f1._promise = move(this->_promise);
		_f1._future = (this->_future).via(executor, priority);
		_f1._t_future = (this->_t_future).via(executor, priority);

		return _f1;
	} 

	// Wrapper(s) for "thenValue" API
	template <typename R, typename... Args>
	auto thenValue(R(&func)(Args...))&& {
		ControlledFuture<R> _f1 = ControlledFuture<R>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValue(&func);

				_f1._t_future = move(_f1._future).thenValue([&](R _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread(); 
				_socket->StartThread(this->_count);
				return _x;
				}).thenValue(&func);

				_f1._t_future = move(_f1._future).thenValue([&](R _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	template <typename F>
	ControlledFuture<typename futures::detail::valueCallableResult<T, F>::value_type>
		thenValue(F&& func)&& {
		ControlledFuture<typename futures::detail::valueCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::valueCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValue(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::valueCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread(); 
				_socket->StartThread(this->_count);
				return _x;
				}).thenValue(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::valueCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}


	// Wrapper(s) for "thenValueInline" API
	template <typename R, typename... Args>
	auto thenValueInline(R(&func)(Args...))&& {
		ControlledFuture<R> _f1 = ControlledFuture<R>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValueInline([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValueInline(&func);

				_f1._t_future = move(_f1._future).thenValueInline([&](R _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValueInline([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValueInline(&func);

				_f1._t_future = move(_f1._future).thenValueInline([&](R _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}


	template <typename F>
	ControlledFuture<typename futures::detail::valueCallableResult<T, F>::value_type> thenValueInline(F&& func)&& {
		ControlledFuture<typename futures::detail::valueCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::valueCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValueInline([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValueInline(func);

				_f1._t_future = move(_f1._future).thenValueInline([&](typename futures::detail::valueCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValueInline([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread();
				_socket->StartThread(this->_count);
				return _x;
				}).thenValueInline(func);

				_f1._t_future = move(_f1._future).thenValueInline([&](typename futures::detail::valueCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	// Wrapper(s) for "thenTry" API
	template <typename F>
	ControlledFuture<typename futures::detail::tryCallableResult<T, F>::value_type>
		thenTry(F&& func)&& {
		ControlledFuture<typename futures::detail::tryCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::tryCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenTry(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread(); 
				_socket->StartThread(this->_count);
				return _x;
				}).thenTry(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	// Wrapper(s) for "thenTryInline" API
	template <typename F>
	ControlledFuture<typename futures::detail::tryCallableResult<T, F>::value_type>
		thenTryInline(F&& func)&& {
		ControlledFuture<typename futures::detail::tryCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::tryCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenTryInline(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread(); 
				_socket->StartThread(this->_count);
				return _x;
				}).thenTryInline(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	// Wrapper(s) for "unit" API
	ControlledFuture<Unit> unit()&& {
		ControlledFuture<Unit> _f1 = ControlledFuture<Unit>(false);
		_f1._future = std::move(*(this->_future)).then();
		return _f1;
	}

	// Wrapper(s) for "then" API
	// Note: Still the wrapper has to be completed for other definitions.
	ControlledFuture<Unit> then()&& {
		ControlledFuture<Unit> _f1 = ControlledFuture<Unit>(false);
		_f1._future = std::move(*(this->_future)).thenValue([](T&&) {});
		return _f1;
	}



	// Wrapper(s) for handling Exception(s).
	// Wrapper(s) for "thenError" API. 
	// Note: There are two definitions for thenError() in folly.
	// Also, thenError is not wrapped with the StarThread() and EndThread() API calls.
	template <class ExceptionType, class F>
	ControlledFuture<Unit> thenError(tag_t<ExceptionType> _t3, F&& func)&& {
		ControlledFuture<Unit> _f1 = ControlledFuture<Unit>(false);
		_f1._future = move(this->_t_future).thenError(_t3, func);
		cout << "check 1" << endl;
		_f1._t_future = move(_f1._future);
		cout << "check 2" << endl;
		return _f1;
	}

	// Wrappers(s) for "thenExValue" API.
	// TODO: Unit Testing
	template <typename F>
	Future<typename futures::detail::valueExecutorCallableResult<T, F>::value_type> thenExValue(F&& func)&& {
		ControlledFuture<typename futures::detail::valueExecutorCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::valueExecutorCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExValue(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::valueExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExValue(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::valueExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	// Wrappers(s) for "thenExValueInline" API.
	// TODO: Unit Testing
	template <typename F>
	ControlledFuture<typename futures::detail::valueExecutorCallableResult<T, F>::value_type> thenExValueInline(F&& func)&& {
		ControlledFuture<typename futures::detail::valueExecutorCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::valueExecutorCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValueInline([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExValueInline(func);

				_f1._t_future = move(_f1._future).thenValueInline([&](typename futures::detail::valueExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValueInline([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExValueInline(func);

				_f1._t_future = move(_f1._future).thenValueInline([&](typename futures::detail::valueExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	// Wrappers(s) for "thenExTry" API.
	// TODO: Unit Testing
	template <typename F>
	ControlledFuture<typename futures::detail::tryExecutorCallableResult<T, F>::value_type>
		thenExTry(F&& func)&& {
		ControlledFuture<typename futures::detail::tryExecutorCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::tryExecutorCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExTry(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExTry(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}

	// Wrappers(s) for "thenExTryInline" API.
	// TODO: Unit Testing
	template <typename F>
	ControlledFuture<typename futures::detail::tryExecutorCallableResult<T, F>::value_type>
		thenExTryInline(F&& func)&& {
		ControlledFuture<typename futures::detail::tryExecutorCallableResult<T, F>::value_type> _f1 = ControlledFuture<typename futures::detail::tryExecutorCallableResult<T, F>::value_type>(false);
		_f1._flag_from_promise = this->_flag_from_promise;
		if (this->_flag)
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExTryInline(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});

				if (!(this->_flag_from_promise))
				{
					T _x = {};
					this->_promise.setValue(_x);
				}
		}
		else
		{
			this->_count = Helpers::Count();
			_f1._future = move(this->_t_future).thenValue([&](T _x) {
				NekaraCppClient* _socket = Helpers::GetTestingServices();
				_socket->CreateThread();
				_socket->StartThread(this->_count);
				return _x;
				}).thenExTryInline(func);

				_f1._t_future = move(_f1._future).thenValue([&](typename futures::detail::tryExecutorCallableResult<T, F>::value_type _x) {
					NekaraCppClient* _socket = Helpers::GetTestingServices();
					_socket->EndThread(this->_count);
					return _x;
					});
		}
		return _f1;
	}


	// Wrapper(s) for "get" API
	template <typename T2 = T>
	T2 get() {
		move(this->_t_future).get();
		return move(this->_future).get();
	}

	// Wrapper(s) for "isReady" API
	bool isReady() const {
		return this->_future.isReady();
	}

	// Wrapper(s) for "hasValue" API
	bool hasValue() const {
		return this->_future.hasValue();
	}

	// Wrapper(s) for "hasException" API
	bool hasException() const {
		return this->_future.hasException();
	}

	// Wrapper(s) for "value" API
	T& value()& {
		return this->_future.value();
	}

	T const& value() const& {
		return this->_future.value();
	}

	T&& value()&& {
		return this->_future.value();
	}

	T const&& value() const&& {
		return this->_future.value();
	}

	// Wrapper(s) for "result" API
	Try<T>& result()& {
		return this->_future.result();
	}

	Try<T> const& result() const& {
		return this->_future.result();
	}

	Try<T>&& result()&& {
		return this->_future.result();
	}

	Try<T> const&& result() const&& {
		return this->_future.result();
	}

	// Wrapper(s) for "cancel" API
	void cancel() {
		this->_future.cancel();
	}

	// Wrapper(s) for "valid" API
	bool valid() const noexcept {
		this->_future.valid();
	}

};
