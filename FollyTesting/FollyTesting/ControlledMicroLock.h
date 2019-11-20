#pragma once

#include "TestingServicesClient.h"
#include <folly/synchronization/SmallLocks.h>

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
		TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->CreateResource(this->_id);
	}

	~ControlledMicroLock()
	{
		/* TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->DeleteResource(this->_id); */
	}

	void init()
	{
		_ml.init();
	}

	void lock()
	{

		TestingServicesClient* _socket = Helpers::GetTestingServices();
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
		TestingServicesClient* _socket = Helpers::GetTestingServices();
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
		TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->CreateResource(this->_id);
	}

	~ControlledMicroSpinLock()
	{
		/* TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->DeleteResource(this->_id); */
	}

	void init()
	{
		_ml.init();
	}

	void lock()
	{

		TestingServicesClient* _socket = Helpers::GetTestingServices();
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
		TestingServicesClient* _socket = Helpers::GetTestingServices();
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
		TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->CreateResource(this->_id);
	}

	~ControlledPicoSpinLock()
	{
		/* TestingServicesClient* _socket = Helpers::GetTestingServices();
		_socket->DeleteResource(this->_id); */
	}

	void init(IntType initialValue = 0)
	{
		_ml.init(initialValue);
	}

	void lock()
	{

		TestingServicesClient* _socket = Helpers::GetTestingServices();
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
		TestingServicesClient* _socket = Helpers::GetTestingServices();
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
