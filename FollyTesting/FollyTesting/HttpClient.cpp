#include "HttpClient.h"

HttpClient::HttpClient()
{

}

HttpClient::HttpClient(TestingServicesClientConfig occ)
{
	this->clientID = Helpers::RandomStr();
	this->hSession = WinHttpOpen(L"WinHTTP Example/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (this->hSession)
	{
		// std::cout << "HTTP Session Created. Port: " << occ.GetPort() << std::endl;
		std::wstring hostname = HttpClient::ConvertHostnametoLPCWSTR(occ.GetHost());
		LPCWSTR hostnameLPCWSTR = hostname.c_str();

		this->hConnect = WinHttpConnect(hSession, hostnameLPCWSTR,
			8080, 0);
	}
	else
	{
		std::cout << "Error in creating HTTP Session. Error code: " << GetLastError() << std::endl;
	}

	// To do the initialization 
	// Step 1: Open the POST request
	if (this->hConnect)
	{
		// std::cout << "HTTP Connection made" << std::endl;

		this->hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/rpc/",
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			0);
	}
	else
	{
		std::cout << "Error in Connecting HTTP. Error code: " << GetLastError() << std::endl;
	}

	// Step 2: Sending the Message
	std::string seed = std::to_string(Helpers::RandomInt());
	std::string text = "";

	if (occ.GetSess().compare("") == 0) 
	{
		text = text + "{\"id\":\"req-" + Helpers::RandomStr() + "\",\"sender\":\"" + this->clientID + "\",\"recipient\":\"Tester-Server\",\"func\":\"InitializeTestSession\",\"args\":[";
		text = text + "{\"id\":\"\",\"assemblyName\":\"NekaraTests, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null\",\"assemblyPath\":\"\",";
		text = text + "\"methodDeclaringClass\":\"\",\"methodName\":\"Run\",\"schedulingSeed\":" + seed + ",\"timeoutMs\":5000,\"maxDecisions\":1000}]}";
		//text = text + "Benchmarks, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null\",\"no-path\",\"Benchmarks.Account\", \"RunTest\"," + seed + "]}";
	}
	else
	{
		text = text + "{\"id\":\"req-" + Helpers::RandomStr() + "\",\"sender\":\"" + this->clientID + "\",\"recipient\":\"Tester-Server\",\"func\":\"ReplayTestSession\",\"args\":[\"";
		text = text + occ.GetSess() + "\"]}";
	}

	BOOL bResults = false;

	// std::cout << text << std::endl;

	if (this->hRequest)
	{
		// std::cout << "HTTP Request Successful" << std::endl;

		bResults = WinHttpSendRequest(this->hRequest,
			L"http://localhost:8080/rpc/", -1,
			(LPWSTR)text.c_str(), std::strlen(text.c_str()),
			strlen(text.c_str()), NULL);
	}
	else
	{
		std::cout << "Error in HTTP Request. Error code: " << GetLastError() << std::endl;
	}

	if (bResults) {
		// std::cout << "Message send successfully" << std::endl;
		bResults = WinHttpReceiveResponse(this->hRequest, NULL);
	}
	else
	{
		std::cout << "Error in HTTP sending. Error code: " << GetLastError() << std::endl;
	}

	if (bResults)
	{
		std::string str = HttpClient::ConvertResponseLPCWSTRtoString(this->hRequest);
		std::string str2 = "\"data\":";
		std::size_t found = str.find(str2);
		std::string session = "";

		int convertdata = static_cast<int>(found);

		int z = convertdata + str2.length();
		while (true)
		{
			// std::cout << str[z];
			session = session + str[z];
			z++;
			if (str[z] == '\"')
				break;
		}

		session = session + "\"";
		this->SessionId = session;

		if (occ.GetSess().compare("") != 0) 
		{
			this->SessionId = "\"" + occ.GetSess() + "\"";
		}

		std::cout << "HTTP Session Initialized. Port: " << occ.GetPort() << ". Session ID: " << this->SessionId << std::endl;
	}
	else
	{
		// std::cout << "Message received is Unsuccessful" << std::endl;
	}
	// WinHttpCloseHandle(this->hRequest);
}



void HttpClient::SendRequest(std::string func, std::string json)
{
	HINTERNET  hRequest = WinHttpOpenRequest(this->hConnect, L"POST", L"/rpc/",
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		0);

	// Json string 
	std::string text = "";
	text = text + "{\"id\":\"req-" + Helpers::RandomStr() + "\",\"sender\":\"" + this->clientID + "\",\"recipient\":\"Tester-Server\",\"func\":\"";
	text = text + func + "\",\"args\":[";
	text = text + this->SessionId + json + "]}";

	// std::cout << text << std::endl;

	BOOL bResults = false;

	if (hRequest)
	{
		bResults = WinHttpSendRequest(hRequest,
			L"http://localhost:8080/rpc/", -1,
			(LPWSTR)text.c_str(), std::strlen(text.c_str()),
			strlen(text.c_str()), NULL);
	}
	else
	{
		std::cout << "Error in HTTP Request. Error code: " << GetLastError() << std::endl;
	}

	if (bResults) {
		// std::cout << "Message send successfully" << std::endl;
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	}
	else
	{
		std::cout << "Error in HTTP sending. Error code: " << GetLastError() << std::endl;
	}
		
	if (bResults)
	{
		// std::cout << "The received message: " << HttpClient::ConvertResponseLPCWSTRtoString(hRequest) << std::endl;
		// std::cout << "Response received - Successful" << std::endl;

		std::string str = HttpClient::ConvertResponseLPCWSTRtoString(hRequest);
		std::string str2 = "\"responseTo\":";
		std::size_t found = str.find(str2);
		std::string reqID = "";

		

		int convertdata = static_cast<int>(found);

		int z = convertdata + str2.length();
		while (true)
		{
			reqID = reqID + str[z];
			z++;
			if (str[z] == '\"')
				break;
		}

		reqID = reqID + "\"";
		
		str2 = "\"error\":true";
		found = str.find(str2);
		convertdata = static_cast<int>(found);

		if (convertdata == -1)
		{
			Helpers::Printres(reqID);
			// std::cout << "--> Got Response to request: " << reqID << " error: False." << std::endl;
		}
		else
		{
			// std::cout << "--> Got Response to request: " << reqID << " error: True." << std::endl;
			// std::cout << "The error is: " << str << std::endl;
			std::string errormsg = "";
			str2 = "\"Message\":";
			found = str.find(str2);
			convertdata = static_cast<int>(found);

			z = convertdata + str2.length();
			while (true)
			{
				errormsg = errormsg + str[z];
				z++;
				if (str[z] == '\"')
					break;
			}

			errormsg = errormsg + "\"";

			Helpers::Printreserror(reqID, errormsg);
			
			// std::cout << "The Error: " << errormsg << std::endl;
		}

		// std::cout << "The received message: " << func << ":-:" << str << std::endl;

		// WinHttpCloseHandle(hRequest);
	}
	else 
	{
		std::cout << "Response received - Unuccessful" << std::endl;
	}
	
}


HINTERNET HttpClient::getHRequest()
{
	return this->hRequest;
}

std::wstring HttpClient::ConvertHostnametoLPCWSTR(const std::string& hostname)
{
	int hostnamelength = (int)hostname.length() + 1;
	int length = MultiByteToWideChar(CP_ACP, 0, hostname.c_str(), hostnamelength, 0, 0);
	wchar_t* buf = new wchar_t[length];
	MultiByteToWideChar(CP_ACP, 0, hostname.c_str(), hostnamelength, buf, length);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string HttpClient::ConvertResponseLPCWSTRtoString(HINTERNET hRequest)
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	std::string str = "";

	// std::cout << "Message received is Successful" << std::endl;
	do {
		dwSize = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
		{
			printf("Error %u in WinHttpQueryDataAvailable.\n",
				GetLastError());
			break;
		}

		if (!dwSize)
			break;

		pszOutBuffer = new char[dwSize + 1];
		if (!pszOutBuffer)
		{
			printf("Out of memory\n");
			break;
		}

		ZeroMemory(pszOutBuffer, dwSize + 1);

		if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
			dwSize, &dwDownloaded))
		{
			printf("Error %u in WinHttpReadData.\n", GetLastError());
		}
		else
		{
			// printf("%s-t", pszOutBuffer);
			str = std::string(pszOutBuffer);

			// JSONObject json = (JSONObject)JSONSerializer.toJSON(data);
		}

		delete[] pszOutBuffer;

		if (!dwDownloaded)
			break;

	} while (dwSize > 0);

	return str;
}

HttpClient::~HttpClient()
{
}