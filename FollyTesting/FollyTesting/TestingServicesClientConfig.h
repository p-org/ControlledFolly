#pragma once
#include <string>
#ifndef TestingServicesClientConfig_h
#define TestingServicesClientConfig_h

enum Transport { IPC, HTTP, GRPC, WS, TCP };

class TestingServicesClientConfig
{
private:
	Transport transport;
	std::string host;
	int port;
	std::string sess;

public:
	TestingServicesClientConfig();
	TestingServicesClientConfig(Transport transport, std::string host, int port, std::string sess);
	TestingServicesClientConfig(Transport transport, std::string host, int port);
	Transport GetTransport();
	std::string GetHost();
	int GetPort();
	std::string GetSess();
	~TestingServicesClientConfig();
};

#endif