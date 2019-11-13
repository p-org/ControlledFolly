#include "TestingServicesClientConfig.h"
#include <string>
#include <iostream>

TestingServicesClientConfig::TestingServicesClientConfig()
{
	this->transport = HTTP;
	// std::cout << "I' am called-TestingServicesClientConfig" << std::endl;
}

TestingServicesClientConfig::TestingServicesClientConfig(Transport transport = HTTP, std::string host = "", int port = 0)
{
	// std::cout << "I' am called-TestingServicesClientConfig2" << std::endl;
	this->transport = transport;
	this->host = host;
	this->port = port;
	this->sess = "";
}

TestingServicesClientConfig::TestingServicesClientConfig(Transport transport, std::string host, int port, std::string sess)
{
	// std::cout << "I' am called-TestingServicesClientConfig2" << std::endl;
	this->transport = transport;
	this->host = host;
	this->port = port;
	this->sess = sess;
}

Transport TestingServicesClientConfig::GetTransport()
{
	return this->transport;
}

std::string TestingServicesClientConfig::GetHost()
{
	return this->host;
}

std::string TestingServicesClientConfig::GetSess()
{
	return this->sess;
}

int TestingServicesClientConfig::GetPort()
{
	return this->port;
}

TestingServicesClientConfig::~TestingServicesClientConfig()
{
}


