#pragma once
#include <string>
#ifndef NekaraCppClientConfig_h
#define NekaraCppClientConfig_h

enum Transport { IPC, HTTP, GRPC, WS, TCP };

class NekaraCppClientConfig
{
private:
	Transport transport;
	std::string host;
	int port;
	std::string sess;

public:
	NekaraCppClientConfig();
	NekaraCppClientConfig(Transport transport, std::string host, int port, std::string sess);
	NekaraCppClientConfig(Transport transport, std::string host, int port);
	Transport GetTransport();
	std::string GetHost();
	int GetPort();
	std::string GetSess();
	~NekaraCppClientConfig();
};

#endif