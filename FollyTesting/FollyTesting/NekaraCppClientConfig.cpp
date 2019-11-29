#include "NekaraCppClientConfig.h"
#include <string>
#include <iostream>

NekaraCppClientConfig::NekaraCppClientConfig()
{
	this->transport = HTTP;
	// std::cout << "I' am called-NekaraCppClientConfig" << std::endl;
}

NekaraCppClientConfig::NekaraCppClientConfig(Transport transport = HTTP, std::string host = "", int port = 0)
{
	// std::cout << "I' am called-NekaraCppClientConfig2" << std::endl;
	this->transport = transport;
	this->host = host;
	this->port = port;
	this->sess = "";
}

NekaraCppClientConfig::NekaraCppClientConfig(Transport transport, std::string host, int port, std::string sess)
{
	// std::cout << "I' am called-NekaraCppClientConfig2" << std::endl;
	this->transport = transport;
	this->host = host;
	this->port = port;
	this->sess = sess;
}

Transport NekaraCppClientConfig::GetTransport()
{
	return this->transport;
}

std::string NekaraCppClientConfig::GetHost()
{
	return this->host;
}

std::string NekaraCppClientConfig::GetSess()
{
	return this->sess;
}

int NekaraCppClientConfig::GetPort()
{
	return this->port;
}

NekaraCppClientConfig::~NekaraCppClientConfig()
{
}


