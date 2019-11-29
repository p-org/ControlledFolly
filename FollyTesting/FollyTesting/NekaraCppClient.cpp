#include "NekaraCppClient.h"
#include "HttpClient.h"
#include <iostream>

	NekaraCppClient::NekaraCppClient()
	{
		// std::cout << "I' am called-NekaraCppClient" << std::endl;
	}

	NekaraCppClient::NekaraCppClient(NekaraCppClientConfig config)
	{
		this->config = config;
		this->counter = 0;


		switch (this->config.GetTransport())
		{
		case IPC:
			NekaraCppClient::SetupTransportIPC();
			break;
		case HTTP:
			NekaraCppClient::SetupTransportHTTP();
			break;
		case GRPC:
			NekaraCppClient::SetupTransportGRPC();
			break;
		case WS:
			NekaraCppClient::SetupTransportWS();
			break;
		case TCP:
			NekaraCppClient::SetupTransportTCP();
			break;
		default:
			break;
		}
	}

	void NekaraCppClient::SetupTransportGRPC()
	{

	}

	void NekaraCppClient::SetupTransportHTTP()
	{
		// std::cout << "HTTP-8080:" << std::endl;
		this->_socket = new HttpClient(this->config);

	}

	void NekaraCppClient::SetupTransportIPC()
	{

	}

	void NekaraCppClient::SetupTransportTCP()
	{

	}

	void NekaraCppClient::SetupTransportWS()
	{

	}

	void NekaraCppClient::SendRequest(std::string func, std::string json)
	{
		(this->_socket)->SendRequest(func, json);
	}

	void NekaraCppClient::CreateThread()
	{

		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Enter");
		this->SendRequest("CreateTask", "");
		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Exit");

	}

	void NekaraCppClient::StartThread(int threadID)
	{
		std::string str = "," + std::to_string(threadID);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "StartThread", std::to_string(threadID), "Enter");
		this->SendRequest("StartTask", str);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "StartThread", std::to_string(threadID), "Exit");

	}

	void NekaraCppClient::EndThread(int threadID)
	{
		std::string str = "," + std::to_string(threadID);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "EndThread", std::to_string(threadID), "Enter");
		this->SendRequest("EndTask", str);
		Helpers::Printingstuff(std::to_string(this->getCounter()), "EndThread", std::to_string(threadID), "Exit");
	}

	void NekaraCppClient::ContextSwitch()
	{
		Helpers::Printingstuff(std::to_string(this->getCounter()), "ContextSwitch", "", "Enter");
		this->SendRequest("ContextSwitch", "");
		Helpers::Printingstuff(std::to_string(this->getCounter()), "ContextSwitch", "", "Exit");
	}

	void NekaraCppClient::Assert(bool predicate, std::string s)
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

	void NekaraCppClient::BlockedOnResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		std::cout << this->counter++ << "\t" << "BlockedOnResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("BlockedOnResource", str);
		std::cout << this->counter++ << "\t" << "BlockedOnResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void NekaraCppClient::CreateResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		// std::cout << this->counter++ << "\t" << "CreateResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("CreateResource", str);
		// std::cout << this->counter++ << "\t" << "CreateResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void NekaraCppClient::DeleteResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		std::cout << this->counter++ << "\t" << "DeleteResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("DeleteResource", str);
		std::cout << this->counter++ << "\t" << "DeleteResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void NekaraCppClient::SignalUpdatedResource(int resourceId)
	{
		std::string str = "," + std::to_string(resourceId);
		// std::cout << this->counter++ << "\t" << "SignalUpdatedResource(" << resourceId << ")" << "\t" << "Enter" << std::endl;
		this->SendRequest("SignalUpdatedResource", str);
		// std::cout << this->counter++ << "\t" << "SignalUpdatedResource(" << resourceId << ")" << "\t" << "Exit" << std::endl;
	}

	void NekaraCppClient::WaitForMainTask()
	{
		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Enter");
		this->SendRequest("WaitForMainTask", "");
		Helpers::Printingstuff(std::to_string(this->getCounter()), "CreateThread", "", "Exit");
	}

	int NekaraCppClient::getCounter()
	{
		mu.lock();
		int t = this->counter++;
		mu.unlock();
		return t;
	}

	NekaraCppClient::~NekaraCppClient()
	{
	}

