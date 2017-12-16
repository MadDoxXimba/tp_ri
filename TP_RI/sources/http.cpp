#include <ctime>
#include <sstream>
#include <iomanip>
#include "http.h"
#include "params.h"
#include <iostream>
#include <vector>
#include<map>


#define TAILLE 1024

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

extern ParamIndex params;

void HttpRequest::GetResponse(SOCKET sd)
{
	char buffer[TAILLE];
	int nb;
	SetConsoleOutputCP(GetACP());
	std::string ressource = "";
	std::string message = "";
	int write = 0;
	int countEndOfClientMessage = 0;
	char state;
	while (nb = recv(sd, buffer, TAILLE - 1, 0) > 0) 
	{
		int index = 0;
		while (buffer[index] >= ' ' || buffer[index] == '\n' || buffer[index] == '\r')
		{
			index += 1;
			if (buffer[index] == ' ' && write == 0)
			{
				write = 1;
			}
			else if (write == 1 && buffer[index] != ' ')
			{
				ressource += buffer[index];
			}
			else if (write == 1)
			{
				write = 2;
			}
			else if (buffer[index] == '\r')
			{
				state = '\r';
				countEndOfClientMessage++;
			}
			else if (countEndOfClientMessage == 1 && buffer[index] == '\n' && state == '\r')
			{
				state = '\n';
				countEndOfClientMessage++;
			}
			else if (countEndOfClientMessage == 2 && buffer[index] == '\r' && state == '\n')
			{
				state = '\r';
				countEndOfClientMessage++;
			}
			else if (countEndOfClientMessage == 3 && buffer[index] == '\n' && state == '\r')
			{
				state = '\n';
				std::string link;
				SetHeader(link, "200 OK");
				link += "<html><strong>Bonjour, </strong> vous avez demandé la ressource: <strong style=\"color:red\">" + ressource + "</strong></html>";
				send(sd, link.c_str(), link.size(), 0);
			}
			else
			{
				state = ' ';
				countEndOfClientMessage = 0;
			}
		}
	}

	closesocket(sd);
}

void HttpRequest::SetHeader(std::string &AnswerBuf, char *httpCode, const char *mime)
{
	std::stringstream header;

	// GetTime
	auto t = std::time(nullptr);

	// Set HTTP header
	header << "HTTP/1.0 " << httpCode << "\r\n";
	header << "Date: " << std::put_time(std::gmtime(&t), "%a, %d %b %Y %H:%M:%S GMT") << "\r\n";
	header << "Server: WebSearchServer/1.0" << "\r\n";
	header << "Content-Type: " << mime << "\r\n";
	header << "\r\n";

	AnswerBuf = header.str();
}
