#pragma once
#ifndef HttpClient_h
#define HttpClient_h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winhttp.h>
#include <winnt.h>
#include <minwindef.h>
#include <iostream>
#include <string>
#include "NekaraCppClientConfig.h"
#include "NekaraCppClient.h"
#include "IClientMethods.h"
#include "Helpers.h"

#pragma comment(lib, "winhttp.lib")
class HttpClient : public IClientMethods
{
private:
	HINTERNET  hSession;
	HINTERNET  hConnect;
	HINTERNET  hRequest;
	std::string clientID;
	std::string SessionId;
	std::wstring ConvertHostnametoLPCWSTR(const std::string&);
	std::string ConvertResponseLPCWSTRtoString(HINTERNET hRequest);
public:
	HttpClient();
	HttpClient(NekaraCppClientConfig omcc);
	HINTERNET getHRequest();
	void SendRequest(std::string func, std::string json);
	~HttpClient();
};
#endif // !HttpClient_h

