#pragma once
#ifndef ITestingService_h
#define ITestingService_h

class ITestingService
{
protected:
	virtual void CreateThread() = 0;
	virtual void StartThread(int threadID) = 0;
	virtual void EndThread(int threadId) = 0;
	virtual void ContextSwitch() = 0;
};

#endif // !ITestingService_h

