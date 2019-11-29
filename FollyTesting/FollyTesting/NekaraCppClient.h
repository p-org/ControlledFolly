#pragma once
#ifndef NekaraCppClient_h
#define NekaraCppClient_h
#include "NekaraCppClientConfig.h"
#include "IClientMethods.h"
#include "ITestingService.h"
#include "HttpClient.h"
#include "Helpers.h"
#include <thread>
#include <mutex>
#include <assert.h>
#include <cassert>

	class NekaraCppClient : protected ITestingService
	{
	private:
		NekaraCppClientConfig config;
		IClientMethods* _socket;
		int counter;
		std::mutex mu;


		void SetupTransportIPC();
		void SetupTransportHTTP();
		void SetupTransportGRPC();
		void SetupTransportWS();
		void SetupTransportTCP();
		int getCounter();

	public:
		void CreateThread();
		void StartThread(int threadID);
		void EndThread(int threadId);
		void ContextSwitch();
		void CreateResource(int resourceId);
		void DeleteResource(int resourceId);
		void BlockedOnResource(int resourceId);
		void SignalUpdatedResource(int resourceId);
		void Assert(bool predicate, std::string s);
		void WaitForMainTask();
		NekaraCppClient();
		NekaraCppClient(NekaraCppClientConfig config);
		void SendRequest(std::string func, std::string json);
		~NekaraCppClient();
	};

#endif

