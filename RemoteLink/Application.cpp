#include "Application.h"
#include "Client.h"
#include "Server.h"
#include "imgui.h"

using namespace ImGui;

#define PORT 8080

namespace Application {
	bool isServer = false;
	
	void Update() {
		DockSpaceOverViewport(0);

        Begin("Settings");
		SeparatorText("Server");
		if (Button("Start"))
		{
			isServer = true;
			Server::Start(PORT);
		}

		SeparatorText("Client");
		static char addr[20];
		InputText("Operator Address", addr, IM_ARRAYSIZE(addr));
		
		if (Button("Connect"))
		{
			isServer = false;
			Client::Connect(addr, PORT);
		}
		End();

		Begin("Screen");
		End();

		if (isServer && Server::IsInitialized())
			Server::Update();
		else if (!isServer && Client::IsConnected())
			Client::Update();
	}

	void Close(){
		Server::Close();
		Client::Disconnect();
	}
}