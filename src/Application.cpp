#include <iostream>
#include <mutex>
#include <thread>
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
		int connectPort = atoi(connectPortStr);
		IPAddress address = IPAddress(addr, *new NetworkNumber<unsigned short>(connectPort, NumberType::Host));
		if (Button("Connect"))
		{
			thread clientConnect([](IPAddress address)
			{
				Client::Connect(address);
			}, address);
			clientConnect.detach();
		}

		static char portStr[6];
		InputText("Port", portStr, IM_ARRAYSIZE(portStr));
		NetworkNumber<unsigned short>* port = new NetworkNumber<unsigned short>(atoi(portStr), NumberType::Host);
		if (Button("Open Server"))
		{
			thread serverStart([](NetworkNumber<unsigned short> port)
			{
				Server::Start(port);
			}, *port);
			serverStart.detach();
		}

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

	void Log(string msg, bool isServer)
	{
		mtx.lock();

		if (isServer)
			msg = "[Server] " + msg;
		else
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
