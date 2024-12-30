#include "Application.h"
#include <iostream>
#include <mutex>
#include <thread>

#include "Client.h"
#include "Server.h"
#include "imgui.h"
#include <winsock2.h>
#include "Utils.h"

using namespace ImGui;
using namespace std;

#define PORT 8080

namespace Application {
	string log = "";
    mutex mtx;
	
	int Start()
	{
		if (Utils::SetupWSA() != 0)
			return 1;

		thread serverStart([](int port)
		{
			Server::Start(port);
		}, PORT);
		serverStart.detach();
		
		return 0;
	}
	
	void Update() {
		DockSpaceOverViewport(0);

        Begin("Settings");
		SeparatorText("Connect To Others");
		
		static char addr[20];
		InputText("Address", addr, IM_ARRAYSIZE(addr));
		IPAddress address = IPAddress(addr);
		if (Button("Connect"))
		{
			thread clientConnect([](IPAddress address, int port)
			{
				Client::Connect(address, port);
			}, address, PORT);
			clientConnect.detach();
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
			SameLine();
			Text(client.GetAsString().c_str());
			if (Button("Accept"))
				Server::AcceptConnection();
		}

		SeparatorText("Logs");
		if (!log.empty())
			Text(log.c_str());
		End();

		Begin("Screen");
		End();

		// Server::Update();
		// if (Client::IsConnected())
		// 	Client::Update();
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
