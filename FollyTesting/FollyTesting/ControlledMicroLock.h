#pragma once

#include <folly/synchronization/SmallLocks.h>
#include "NekaraCppClient.h"

using namespace folly;
using namespace std;

class ControlledMicroLock
{
private:
	int _id = Helpers::RandomInt();
	int _lock_status = false;
	MicroLock _ml;
	MicroLock _t_ml;

public:

	ControlledMicroLock() 
	{
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->CreateResource(this->_id);
	}

	~ControlledMicroLock()
	{
		/* NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->DeleteResource(this->_id); */
	}

	void init()
	{
		_ml.init();
	}

	void lock()
	{

		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->ContextSwitch();

		//TODO: The below while have to be efficiently re-coded
		while (true)
		{
			_t_ml.lock();
			if (!this->_lock_status)
			{
				this->_lock_status = true;
				_t_ml.unlock();
				break;
			}
			_t_ml.unlock();
			_socket->BlockedOnResource(this->_id);
		}
		_ml.lock();
	}

	void unlock()
	{
		_ml.unlock();
		_t_ml.lock();
		this->_lock_status = false;
		_t_ml.unlock();
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->SignalUpdatedResource(this->_id);
	}

	void try_lock()
	{
		_ml.try_lock();
	}
};



class ControlledMicroSpinLock
{
private:
	int _id = Helpers::RandomInt();
	int _lock_status = false;
	MicroSpinLock _ml;
	MicroLock _t_ml;

public:
	enum { FREE = 0, LOCKED = 1 };

	ControlledMicroSpinLock()
	{
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->CreateResource(this->_id);
	}

	~ControlledMicroSpinLock()
	{
		/* NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->DeleteResource(this->_id); */
	}

	void init()
	{
		_ml.init();
	}

	void lock()
	{

		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->ContextSwitch();

		//TODO: The below while have to be efficiently re-coded
		while (true)
		{
			_t_ml.lock();
			if (!this->_lock_status)
			{
				this->_lock_status = true;
				_t_ml.unlock();
				break;
			}
			_t_ml.unlock();
			_socket->BlockedOnResource(this->_id);
		}
		_ml.lock();
	}

	void unlock()
	{
		_ml.unlock();
		_t_ml.lock();
		this->_lock_status = false;
		_t_ml.unlock();
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->SignalUpdatedResource(this->_id);
	}

	void try_lock()
	{
		_ml.try_lock();
	}
};


template <class IntType, int Bit = sizeof(IntType) * 8 - 1>
class ControlledPicoSpinLock
{
private:
	int _id = Helpers::RandomInt();
	int _lock_status = false;
	PicoSpinLock<IntType> _ml;
	MicroLock _t_ml;

public:

	ControlledPicoSpinLock()
	{
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->CreateResource(this->_id);
	}

	~ControlledPicoSpinLock()
	{
		/* NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->DeleteResource(this->_id); */
	}

	void init(IntType initialValue = 0)
	{
		_ml.init(initialValue);
	}

	void lock()
	{

		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->ContextSwitch();

		//TODO: The below while have to be efficiently re-coded
		while (true)
		{
			_t_ml.lock();
			if (!this->_lock_status)
			{
				this->_lock_status = true;
				_t_ml.unlock();
				break;
			}
			_t_ml.unlock();
			_socket->BlockedOnResource(this->_id);
		}
		_ml.lock();
	}

	void unlock()
	{
		_ml.unlock();
		_t_ml.lock();
		this->_lock_status = false;
		_t_ml.unlock();
		NekaraCppClient* _socket = Helpers::GetTestingServices();
		_socket->SignalUpdatedResource(this->_id);
	}

	void try_lock()
	{
		_ml.try_lock();
	}

	IntType getData() {
		return _ml.getData();
	}

	void setData(IntType w) {
		_ml.setData(w);
	}
};
