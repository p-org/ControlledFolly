#pragma once
#include "ControlledFuture.h"
#include <folly/futures/Future.h>

template <typename T>
class ControlledFuture;

using namespace folly;
using namespace std;

template <class T>
class ControlledPromise {
public:
	Promise<T> _promise;

	ControlledPromise()
	{
	}

	ControlledFuture<T> getFuture()
	{
		ControlledFuture<T> _f1 = ControlledFuture<T>();
		this->_promise = move(_f1._promise);
		_f1._flag_from_promise = true;
		return _f1;
	}


	void setValue()
	{
		this->_promise.setValue();
	}

	void setValue(T _x)
	{
		this->_promise.setValue(_x);
	}

	
	bool isFulfilled() {
		return this->_promise.isFulfilled();
	}

	bool valid() {
		return this->_promise.valid();
	}
};
