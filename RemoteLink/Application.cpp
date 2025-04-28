#include <iostream>
#include <mutex>
#include <thread>
#include <winsock2.h>

#include "Application.h"
#include "Client.h"
#include "Server.h"
#include "imgui.h"
#include "Utils.h"

using namespace ImGui;
using namespace std;

namespace Application {
	string log;
    mutex mtx;
	
	int Start()
	{
		if (Utils::SetupWSA() != 0)
			return 1;
		
		return 0;
	}
	
	void Update() {
		DockSpaceOverViewport(0);

        Begin("Settings");
		SeparatorText("Connect To Others");
		
		static char addr[20];
		InputText("Address", addr, IM_ARRAYSIZE(addr));
		static char connectPortStr[6];
		InputText("Connect Port", connectPortStr, IM_ARRAYSIZE(connectPortStr));
		IPAddress address = IPAddress(addr);
		int connectPort = atoi(connectPortStr);
		if (Button("Connect"))
		{
			thread clientConnect([](IPAddress address, int port)
			{
				Client::Connect(address, port);
			}, address, connectPort);
			clientConnect.detach();
		}

		static char portStr[6];
		InputText("Port", portStr, IM_ARRAYSIZE(portStr));
		int port = atoi(portStr);
		if (Button("Open Server"))
		{
			thread serverStart([](int port)
			{
				Server::Start(port);
			}, port);
			serverStart.detach();
		}

		// static char dat[20];
		// InputText("Data", dat, IM_ARRAYSIZE(dat));
		// string data = dat;
		// if (Button("Send Data"))
		// 	Client::SendData(data);

		SeparatorText("Incoming Requests");
		IPAddress client;
		if (Server::IsRequested(&client))
		{
			Text(client.GetAsString().c_str());
			SameLine();
			if (Button("Accept"))
				Server::AcceptConnection();
		}
		
		SeparatorText("Logs");
		if (!log.empty())
			Text(log.c_str());
		End();

		Begin("Screen");
		SeparatorText("Chat");
		static char message[25];
		InputText("Message", message, IM_ARRAYSIZE(message));
		if (Button("Send Message"))
		{
			if (Client::IsConnected())
				Client::SendMessageToServer(message);
		}
		End();
	}

	void Log(string msg, BOOL isServer)
	{
		mtx.lock();
		
		if (isServer == TRUE)
			msg = "[Server] " + msg;
		else if (isServer == FALSE)
			msg = "[Client] " + msg;
		
		cout << msg << endl;
		if (log.empty())
			log = msg;
		else
			log += "\n" + msg;

		mtx.unlock();
	}

	void Close(){
		Server::Close();
		Client::Disconnect();
	}
}
