#include "TestingServicesClient.h"
#include "HttpClient.h"
#include <iostream>

	TestingServicesClient::TestingServicesClient()
	{
		// std::cout << "I' am called-TestingServicesClient" << std::endl;
	}

	TestingServicesClient::TestingServicesClient(TestingServicesClientConfig config)
	{
		this->config = config;
		this->counter = 0;


		switch (this->config.GetTransport())
		{
		case IPC:
			TestingServicesClient::SetupTransportIPC();
			break;
		case HTTP:
			TestingServicesClient::SetupTransportHTTP();
			break;
		case GRPC:
			TestingServicesClient::SetupTransportGRPC();
			break;
		case WS:
			TestingServicesClient::SetupTransportWS();
			break;
		case TCP:
			TestingServicesClient::SetupTransportTCP();
			break;
		default:
			break;
		}
	}

	void TestingServicesClient::SetupTransportGRPC()
	{

	}

	void TestingServicesClient::SetupTransportHTTP()
	{
		// std::cout << "HTTP-8080:" << std::endl;
		this->_socket = new HttpClient(this->config);

	}

	void TestingServicesClient::SetupTransportIPC()
	{

	}

	void TestingServicesClient::SetupTransportTCP()
	{

	}

	void TestingServicesClient::SetupTransportWS()
	{

	}

	void TestingServicesClient::SendRequest(std::string func, std::string json)
	{
		(this->_socket)->SendRequest(func, json);
	}

	void TestingServicesClient::CreateThread()
	{

		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Enter");
		this->SendRequest("CreateTask", "");
		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Exit");

	}

	void TestingServicesClient::StartThread(int threadID)
	{
		std::string str = "," + std::to_string(threadID);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "StartThread", std::to_string(threadID), "Enter");
		this->SendRequest("StartTask", str);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "StartThread", std::to_string(threadID), "Exit");

	}

	void TestingServicesClient::EndThread(int threadID)
	{
		std::string str = "," + std::to_string(threadID);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "EndThread", std::to_string(threadID), "Enter");
		this->SendRequest("EndTask", str);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "EndThread", std::to_string(threadID), "Exit");
	}

	void TestingServicesClient::ContextSwitch()
	{
		Helpers::Printingstuff(std::to_string(this->getCounter()), "ContextSwitch", "", "Enter");
		this->SendRequest("ContextSwitch", "");
		Helpers::Printingstuff(std::to_string(this->getCounter()), "ContextSwitch", "", "Exit");
	}

	void TestingServicesClient::Assert(bool predicate, std::string s)
	{
		std::string str = "";
		if (predicate)
		{
			str = ",true,\"" + s + "\"";
		}
		else
		{
			str = ",false,\"" + s + "\"";
		}


		Helpers::Printingstuff(std::to_string(this->getCounter()), "Assert", "", "Enter");
		this->SendRequest("Assert", str);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "Assert", "", "Exit");
	}

	void TestingServicesClient::BlockedOnResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		std::cout << this->counter++ << "\t" << "BlockedOnResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("BlockedOnResource", str);
		std::cout << this->counter++ << "\t" << "BlockedOnResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void TestingServicesClient::CreateResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		std::cout << this->counter++ << "\t" << "CreateResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("CreateResource", str);
		std::cout << this->counter++ << "\t" << "CreateResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void TestingServicesClient::DeleteResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		std::cout << this->counter++ << "\t" << "DeleteResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("DeleteResource", str);
		std::cout << this->counter++ << "\t" << "DeleteResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void TestingServicesClient::SignalUpdatedResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		std::cout << this->counter++ << "\t" << "SignalUpdatedResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("SignalUpdatedResource", str);
		std::cout << this->counter++ << "\t" << "SignalUpdatedResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void TestingServicesClient::WaitForMainTask()
	{
		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Enter");
		this->SendRequest("WaitForMainTask", "");
		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Exit");
	}

	int TestingServicesClient::getCounter()
	{
		mu.lock();
		int t = this->counter++;
		mu.unlock();
		return t;
	}

	TestingServicesClient::~TestingServicesClient()
	{
	}

