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
	void init()
	{
		_ml.init();
	}

	void lock()
	{

		TestingServicesClient* socket = Helpers::GetTestingServices();
		socket->ContextSwitch();

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
			socket->BlockedOnResource(this->_id);
		}
		_ml.lock();
	}

	void unlock()
	{
		_ml.unlock();
		_t_ml.lock();
		this->_lock_status = false;
		_t_ml.unlock();
		TestingServicesClient* socket = Helpers::GetTestingServices();
		socket->SignalUpdatedResource(this->_id);
	}

	void try_lock()
	{
		_ml.try_lock();
	}
};

