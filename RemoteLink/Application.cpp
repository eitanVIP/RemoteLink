#include "Application.h"

#include <iostream>

#include "Client.h"
#include "Server.h"
#include "imgui.h"

using namespace ImGui;
using namespace std;

#define PORT 8080

namespace Application {
	string log = "";
	
	int Start()
	{
		return Server::Start(PORT);
	}
	
	void Update() {
		DockSpaceOverViewport(0);

        Begin("Settings");
		SeparatorText("Connect To Others");
		
		static char addr[20];
		InputText("Address", addr, IM_ARRAYSIZE(addr));
		string address = addr;
		if (Button("Connect"))
			Client::Connect(address, PORT);

		static char dat[20];
		InputText("Data", dat, IM_ARRAYSIZE(dat));
		string data = dat;
		if (Button("Send Data"))
			Client::SendData(data);

		SeparatorText("Incoming Requests");

		SeparatorText("Logs");
		if (!log.empty())
			Text(log.c_str());
		End();

		Begin("Screen");
		End();

		Server::Update();
		if (Client::IsConnected())
			Client::Update();
	}

	void Log(string msg)
	{
		cout << msg << endl;
		if (log.empty())
			log = msg;
		else
			log += "\n" + msg;
	}

	void Close(){
		Server::Close();
		Client::Disconnect();
	}
}