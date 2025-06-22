#include <iostream>
#include <mutex>
#include <thread>
#include "Application.h"

#include <GL/gl.h>

#include "Client.h"
#include "Server.h"
#include "imgui.h"
#include "Utils.h"
#include "Image.h"

using namespace std;

namespace Application {
	string log;
    mutex mtx;
	Server server;
	Client client;
	Image lastImage;
	GLuint textureID;
	
	int Start()
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 720.0 * 1600.0 / 900.0, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		return 0;
	}
	
	void Update() {
		ImGui::DockSpaceOverViewport(0);

        ImGui::Begin("Settings");
		ImGui::SeparatorText("Connect To Others");

		// static char addr[20];
		// ImGui::InputText("Address", addr, IM_ARRAYSIZE(addr));
		// static char connectPortStr[6];
		// ImGui::InputText("Connect Port", connectPortStr, IM_ARRAYSIZE(connectPortStr));
		// int connectPort = atoi(connectPortStr);
		IPAddress address = IPAddress("127.0.0.1", *new NetworkNumber<Port>(8080, NumberType::Host));
		if (ImGui::Button("Connect"))
		{
			thread clientConnect([](IPAddress address)
			{
				client.Connect(address);
			}, address);
			clientConnect.detach();
		}

		// static char portStr[6];
		// ImGui::InputText("Port", portStr, IM_ARRAYSIZE(portStr));
		NetworkNumber<Port>* port = new NetworkNumber<Port>(atoi("8080"), NumberType::Host);
		if (ImGui::Button("Open Server"))
		{
			thread serverStart([](NetworkNumber<Port> port)
			{
				server.Start(port);
			}, *port);
			serverStart.detach();
		}

		ImGui::SeparatorText("Incoming Requests");
		IPAddress clientAddr;
		if (server.IsRequested(&clientAddr))
		{
			ImGui::Text(clientAddr.GetAsString().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Accept"))
				server.AcceptConnection();
		}

		ImGui::SeparatorText("Actions");
		if (ImGui::Button("Send Screenshot"))
			server.SendScreenshot();

		if (server.IsConnected())
		{
			if (ImGui::Button("End Connection"))
				server.Finish();
		}

		if (client.IsConnected())
		{
			if (ImGui::Button("End Connection"))
				client.Finish();
		}

		ImGui::SeparatorText("Logs");
		if (!log.empty())
			ImGui::Text(log.c_str());

		ImGui::End();

		ImGui::Begin("Screen");

		if (!client.GetImages().empty())
		{
			Log("Popped image from queue");
			lastImage = client.GetImages().front();
			lastImage = lastImage.Resize(720);
			client.GetImages().pop();
		}

		if (!lastImage.GetPixels().empty())
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, lastImage.GetWidth(), lastImage.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, lastImage.GetValues().data());

			ImGui::Image(textureID, ImVec2(lastImage.GetWidth(), lastImage.GetHeight()));
		}

		ImGui::End();
	}

	void Log(string msg)
	{
		mtx.lock();

		// if (isServer)
		// 	msg = "[Server] " + msg;
		// else
		// 	msg = "[Client] " + msg;

		cout << msg << endl;
		if (log.empty())
			log = msg;
		else
			log += "\n" + msg;

		mtx.unlock();
	}

	void Close(){
		server.Finish();
		client.Finish();
	}
}
